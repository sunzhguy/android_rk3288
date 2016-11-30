
#include "trust_merger.h"

bool gDebug =
#ifdef DEBUG
	true;
#else
	false;
#endif /* DEBUG */

static char* gConfigPath = NULL;
static OPT_T gOpts;
#define BL3X_FILESIZE_MAX	(512 * 1024)
static char gBuf[BL3X_FILESIZE_MAX];

const uint8_t gBl3xID[BL_MAX_SEC][4] = {
	{'B', 'L', '3', '0'},
	{'B', 'L', '3', '1'},
	{'B', 'L', '3', '2'},
	{'B', 'L', '3', '3'}
};

static inline uint32_t getBCD(uint16_t value)
{
	uint8_t tmp[2] = {0};
	int i;
	uint32_t ret;

	if (value > 0xFFFF) {
		return 0;
	}

	for(i=0; i < 2; i++) {
		tmp[i] = (((value/10)%10)<<4) | (value%10);
		value /= 100;
	}
	ret = ((uint16_t)(tmp[1] << 8)) | tmp[0];

	LOGD("ret:%x\n",ret);
	return ret&0xFF;
}


static inline void fixPath(char* path)
{
	int i, len = strlen(path);

	for(i=0; i<len; i++) {
		if (path[i] == '\\')
			path[i] = '/';
		else if (path[i] == '\r' || path[i] == '\n')
			path[i] = '\0';
	}
}


static bool parseVersion(FILE* file)
{
	SCANF_EAT(file);
	if (fscanf(file, OPT_MAJOR "=%d", &gOpts.major) != 1)
		return false;
	SCANF_EAT(file);
	if (fscanf(file, OPT_MINOR "=%d", &gOpts.minor) != 1)
		return false;
	LOGD("major:%d, minor:%d\n", gOpts.major, gOpts.minor);
	return true;
}


static bool parseBL3x(FILE* file, int bl3x_id)
{
	int pos;
	char buf[MAX_LINE_LEN];
	bl_entry_t *pbl3x = NULL;

	if (bl3x_id >= BL_MAX_SEC) {
		return false;
	}

	pbl3x = &gOpts.bl3x[bl3x_id];

	/* SEC */
	SCANF_EAT(file);
	if (fscanf(file, OPT_SEC "=%d", &pbl3x->sec) != 1)
		return false;
	LOGD("bl3%d sec: %d\n", bl3x_id, pbl3x->sec);
	if (pbl3x->sec == false)
		return true;

	/* PATH */
	SCANF_EAT(file);
	if (fscanf(file, OPT_PATH "=%s", buf) != 1)
		return false;
	fixPath(buf);
	strcpy(pbl3x->path, buf);
	LOGD("bl3%d path:%s\n", bl3x_id, pbl3x->path);

	/* ADDR */
	SCANF_EAT(file);
	if (fscanf(file, OPT_ADDR "=%s", buf) != 1)
		return false;
	pbl3x->addr = strtoul(buf, NULL, 16);
	LOGD("bl3%d addr:0x%x\n", bl3x_id, pbl3x->addr);

	pos = ftell(file);
	SCANF_EAT(file);

	return true;
}


static bool parseOut(FILE* file)
{
	SCANF_EAT(file);
	if (fscanf(file, OPT_OUT_PATH "=%[^\r^\n]", gOpts.outPath) != 1)
		return false;
	fixPath(gOpts.outPath);
	printf("out:%s\n", gOpts.outPath);

	return true;
}


void printOpts(FILE* out)
{
	fprintf(out, SEC_BL30 "\n" OPT_SEC "=%d\n", gOpts.bl3x[BL30_SEC].sec);
	if (gOpts.bl3x[BL30_SEC].sec != false) {
		fprintf(out, OPT_PATH "=%s\n", gOpts.bl3x[BL30_SEC].path);
		fprintf(out, OPT_ADDR "=0x%08x\n", gOpts.bl3x[BL30_SEC].addr);
	}

	fprintf(out, SEC_BL31 "\n" OPT_SEC "=%d\n", gOpts.bl3x[BL31_SEC].sec);
	if (gOpts.bl3x[BL31_SEC].sec != false) {
		fprintf(out, OPT_PATH "=%s\n", gOpts.bl3x[BL31_SEC].path);
		fprintf(out, OPT_ADDR "=0x%08x\n", gOpts.bl3x[BL31_SEC].addr);
	}

	fprintf(out, SEC_BL32 "\n" OPT_SEC "=%d\n", gOpts.bl3x[BL32_SEC].sec);
	if (gOpts.bl3x[BL32_SEC].sec != false) {
		fprintf(out, OPT_PATH "=%s\n", gOpts.bl3x[BL32_SEC].path);
		fprintf(out, OPT_ADDR "=0x%08x\n", gOpts.bl3x[BL32_SEC].addr);
	}

	fprintf(out, SEC_BL33 "\n" OPT_SEC "=%d\n", gOpts.bl3x[BL33_SEC].sec);
	if (gOpts.bl3x[BL33_SEC].sec != false) {
		fprintf(out, OPT_PATH "=%s\n", gOpts.bl3x[BL33_SEC].path);
		fprintf(out, OPT_ADDR "=0x%08x\n", gOpts.bl3x[BL33_SEC].addr);
	}

	fprintf(out, SEC_OUT "\n" OPT_OUT_PATH "=%s\n", gOpts.outPath);
}


static bool parseOpts(void)
{
	FILE* file = NULL;
	char* configPath = (gConfigPath == NULL)? DEF_CONFIG_FILE: gConfigPath;
	bool bl30ok = false, bl31ok = false, bl32ok = false, bl33ok = false;
	bool outOk = false;
	bool versionOk = false;
	char buf[MAX_LINE_LEN];
	bool ret = false;

	file = fopen(configPath, "r");
	if (!file) {
		fprintf(stderr, "config(%s) not found!\n", configPath);
		if (configPath == (char*)DEF_CONFIG_FILE) {
			file = fopen(DEF_CONFIG_FILE, "w");
			if (file) {
				fprintf(stderr, "create defconfig\n");
				printOpts(file);
			}
		}
		goto end;
	}

	LOGD("start parse\n");

	SCANF_EAT(file);
	while(fscanf(file, "%s", buf) == 1) {
		if (!strcmp(buf, SEC_VERSION)) {
			versionOk = parseVersion(file);
			if (!versionOk) {
				LOGE("parseVersion failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_BL30)) {
			bl30ok = parseBL3x(file, BL30_SEC);
			if (!bl30ok) {
				LOGE("parseBL30 failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_BL31)) {
			bl31ok = parseBL3x(file, BL31_SEC);
			if (!bl31ok) {
				LOGE("parseBL31 failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_BL32)) {
			bl32ok = parseBL3x(file, BL32_SEC);
			if (!bl32ok) {
				LOGE("parseBL32 failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_BL33)) {
			bl33ok = parseBL3x(file, BL33_SEC);
			if (!bl33ok) {
				LOGE("parseBL33 failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_OUT)) {
			outOk = parseOut(file);
			if (!outOk) {
				LOGE("parseOut failed!\n");
				goto end;
			}
		} else if (buf[0] == '#') {
			continue;
		} else {
			LOGE("unknown sec: %s!\n", buf);
			goto end;
		}
		SCANF_EAT(file);
	}

	if (bl30ok && bl31ok && bl32ok && bl33ok && outOk)
		ret = true;
end:
	if (file)
		fclose(file);

	return ret;
}

bool initOpts(void)
{
	//set default opts
	memset(&gOpts, 0, sizeof(gOpts));

	gOpts.major = DEF_MAJOR;
	gOpts.minor = DEF_MINOR;

	memcpy(&gOpts.bl3x[BL30_SEC].id, gBl3xID[BL30_SEC], 4);
	strcpy(gOpts.bl3x[BL30_SEC].path, DEF_BL30_PATH);

	memcpy(&gOpts.bl3x[BL31_SEC].id, gBl3xID[BL31_SEC], 4);
	strcpy(gOpts.bl3x[BL31_SEC].path, DEF_BL31_PATH);

	memcpy(&gOpts.bl3x[BL32_SEC].id, gBl3xID[BL32_SEC], 4);
	strcpy(gOpts.bl3x[BL32_SEC].path, DEF_BL32_PATH);

	memcpy(&gOpts.bl3x[BL33_SEC].id, gBl3xID[BL33_SEC], 4);
	strcpy(gOpts.bl3x[BL33_SEC].path, DEF_BL33_PATH);

	strcpy(gOpts.outPath, DEF_OUT_PATH);

	return parseOpts();
}


static inline bool getFileSize(const char *path, uint32_t* size)
{
	struct stat st;

	if (stat(path, &st) < 0)
		return false;
	*size = st.st_size;
	LOGD("path:%s, size:%d\n", path, *size);
	return true;
}


static bool mergetrust(void)
{
	FILE		*outFile = NULL;
	uint32_t 	OutFileSize;
	uint32_t	SrcFileNum, SignOffset;
	TRUST_HEADER	*pHead = NULL;
	COMPONENT_DATA	*pComponentData = NULL;
	TRUST_COMPONENT	*pComponent = NULL;
	uint32_t	filesize[BL_MAX_SEC] = {0};
	uint32_t	imagesize[BL_MAX_SEC] = {0};
	bool		ret = false;
	uint32_t	i;

	if (!initOpts())
		return false;

	if (gDebug) {
		printf("---------------\nUSING CONFIG:\n");
		printOpts(stdout);
		printf("---------------\n\n");
	}

	SrcFileNum = 0;
	for (i = BL30_SEC; i < BL_MAX_SEC; i++) {
		filesize[i] = 0;
		imagesize[i] = 0;

		if (gOpts.bl3x[i].sec != false) {
			if (!getFileSize(gOpts.bl3x[i].path, &filesize[i])) {
				LOGE("open %s file failed\n", gOpts.bl3x[i].path);
				goto end;
			}

			imagesize[i] = ENTRY_ALIGN * ((filesize[i] + ENTRY_ALIGN - 1) / ENTRY_ALIGN);
			if (imagesize[i] > BL3X_FILESIZE_MAX) {
				LOGE("file %s too large.\n", gOpts.bl3x[i].path);
				goto end;
			}

			LOGD("bl3%d: filesize = %d, imagesize = %d\n", i, filesize[i], imagesize[i]);

			SrcFileNum++;
		}
	}
	LOGD("bl3x bin sec = %d\n", SrcFileNum);

	/* 2048bytes for head */
	memset(gBuf, 0, TRUST_HEADER_SIZE);

	/* Trust Head */
	pHead = (TRUST_HEADER *)gBuf;
	memcpy(&pHead->tag, TRUST_HEAD_TAG, 4);
	pHead->version = (getBCD(gOpts.major) << 8) | getBCD(gOpts.minor);
	pHead->flags = 0;
	SignOffset = sizeof(TRUST_HEADER) + SrcFileNum * sizeof(COMPONENT_DATA);
	LOGD("trust bin sign offset = %d\n", SignOffset);
	pHead->size = (SrcFileNum<<16) | (SignOffset>>2);

	pComponent = (TRUST_COMPONENT *)(gBuf + SignOffset + SIGNATURE_SIZE);
	pComponentData = (COMPONENT_DATA *)(gBuf + sizeof(TRUST_HEADER));

	OutFileSize = TRUST_HEADER_SIZE;
	for (i = BL30_SEC; i < BL_MAX_SEC; i++) {
		if (gOpts.bl3x[i].sec != false) {
			/* bl3x load and run address */
			pComponentData->LoadAddr = gOpts.bl3x[i].addr;

			pComponent->ComponentID = gOpts.bl3x[i].id;
			pComponent->StorageAddr = (OutFileSize >> 9);
			pComponent->ImageSize = (imagesize[i] >> 9);

			LOGD("bl3%d: LoadAddr = 0x%08x, StorageAddr = %d, ImageSize = %d\n",\
				i, pComponentData->LoadAddr, pComponent->StorageAddr, pComponent->ImageSize);

			OutFileSize += imagesize[i];
			pComponentData++;
			pComponent++;
		}
	}

	/* create out file */
	outFile = fopen(gOpts.outPath, "wb+");
	if (!outFile) {
		LOGE("open out file(%s) failed\n", gOpts.outPath);

		outFile = fopen(DEF_OUT_PATH, "wb");
		if (!outFile) {
			LOGE("open default out file:%s failed!\n", DEF_OUT_PATH);
			goto end;
		}
	}

	/* save trust head to out file */
	if (!fwrite(gBuf, TRUST_HEADER_SIZE, 1, outFile))
		goto end;

	/* save trust bl3x bin */
	for (i = BL30_SEC; i < BL_MAX_SEC; i++) {
		if (gOpts.bl3x[i].sec != false) {
			FILE *inFile = fopen(gOpts.bl3x[i].path, "rb");
			if (!inFile)
				goto end;

			memset(gBuf, 0, imagesize[i]);
			if (!fread(gBuf, filesize[i], 1, inFile))
				goto end;
			fclose(inFile);

			if (!fwrite(gBuf, imagesize[i], 1, outFile))
				goto end;
		}
	}

	ret = true;
end:
	if (outFile)
		fclose(outFile);
	return ret;
}


static int saveDatatoFile(char* FileName, void *pBuf, uint32_t size)
{
	FILE   *OutFile = NULL;
	int ret = -1;

	OutFile = fopen(FileName, "wb");
	if (!OutFile) {
		printf("open OutPutFlie:%s failed!\n", FileName);
		goto end;
	}
	if (1 != fwrite(pBuf, size, 1, OutFile)) {
		printf("write output file failed!\n");
		goto end;
	}

	ret = 0;
end:
	if (OutFile)
		fclose(OutFile);

	return ret;
}


static bool unpacktrust(char *path)
{
	FILE		*FileSrc = NULL;
	uint32_t	FileSize;
	uint8_t		*pBuf = NULL;
	uint32_t	SrcFileNum, SignOffset;
	TRUST_HEADER	*pHead = NULL;
	COMPONENT_DATA	*pComponentData = NULL;
	TRUST_COMPONENT	*pComponent = NULL;
	char		str[MAX_LINE_LEN];
	bool		ret = false;
	uint32_t	i;

	FileSrc = fopen(path, "rb");
	if (FileSrc == NULL) {
		printf("open %s failed!\n", path);
		goto end;
	}

	if (getFileSize(FileSrc, &FileSize) == false) {
		printf("File Size failed!\n");
		goto end;
	}
	printf("File Size = %d\n", FileSize);

	pBuf = (uint8_t *)malloc(FileSize);
	if(1 != fread(pBuf, FileSize, 1, FileSrc)) {
		printf("read input file failed!\n");
		goto end;
	}

	pHead = (TRUST_HEADER *)pBuf;

	memcpy(str, &pHead->tag, 4);
	str[4] = '\0';
	printf("Header Tag:%s\n", str);
	printf("Header version:%d\n", pHead->version);
	printf("Header flag:%d\n", pHead->flags);

	SrcFileNum = (pHead->size>>16) & 0xffff;
	SignOffset = (pHead->size & 0xffff)<<2;
	printf("SrcFileNum:%d\n", SrcFileNum);
	printf("SignOffset:%d\n", SignOffset);

	pComponent = (TRUST_COMPONENT *)(pBuf + SignOffset + SIGNATURE_SIZE);
	pComponentData = (COMPONENT_DATA *)(pBuf + sizeof(TRUST_HEADER));

	for (i=0; i<SrcFileNum; i++) {
		printf("Component %d:\n", i);

		memcpy(str, &pComponent->ComponentID, 4);
		str[4] = '\0';
		printf("ComponentID:%s\n", str);
		printf("StorageAddr:0x%x\n", pComponent->StorageAddr);
		printf("ImageSize:0x%x\n", pComponent->ImageSize);

		printf("LoadAddr:0x%x\n", pComponentData->LoadAddr);

		saveDatatoFile(str, pBuf+(pComponent->StorageAddr<<9), pComponent->ImageSize<<9);

		pComponentData++;
		pComponent++;
	}

	ret = true;
end:
	if (FileSrc)
		fclose(FileSrc);
	if (pBuf)
		free(pBuf);

	return ret;
}


static void printHelp(void)
{
	printf("Usage: trust_merger [options]... FILE\n");
	printf("Merge or unpack Rockchip's trust image (Default action is to merge.)\n");
	printf("Options:\n");
	printf("\t" OPT_MERGE "\t\t\tMerge trust with specified config.\n");
	printf("\t" OPT_UNPACK "\t\tUnpack specified trust to current dir.\n");
	printf("\t" OPT_VERBOSE "\t\tDisplay more runtime informations.\n");
	printf("\t" OPT_HELP "\t\t\tDisplay this information.\n");
	printf("\t" OPT_VERSION "\t\tDisplay version information.\n");
}


int main(int argc, char** argv)
{
	bool merge = true;
	char* optPath = NULL;
	int i;

	gConfigPath = NULL;
	for(i=1; i<argc; i++) {
		if (!strcmp(OPT_VERBOSE, argv[i])) {
			gDebug = true;
		} else if (!strcmp(OPT_HELP, argv[i])) {
			printHelp();
			return 0;
		} else if (!strcmp(OPT_VERSION, argv[i])) {
			printf("trust_merger (cwz@rock-chips.com)\t" VERSION "\n");
			return 0;
		} else if (!strcmp(OPT_MERGE, argv[i])) {
			merge = true;
		} else if (!strcmp(OPT_UNPACK, argv[i])) {
			merge = false;
		} else {
			if (optPath) {
				fprintf(stderr, "only need one path arg, but we have:\n%s\n%s.\n", optPath, argv[i]);
				printHelp();
				return -1;
			}
			optPath = argv[i];
		}
	}
	if (!merge && !optPath) {
		fprintf(stderr, "need set out path to unpack!\n");
		printHelp();
		return -1;
	}

	if (merge) {
		LOGD("do_merge\n");
		gConfigPath = optPath;
		if (!mergetrust()) {
			fprintf(stderr, "merge failed!\n");
			return -1;
		}
		printf("merge success(%s)\n", gOpts.outPath);
	} else {
		LOGD("do_unpack\n");
		if (!unpacktrust(optPath)) {
			fprintf(stderr, "unpack failed!\n");
			return -1;
		}
		printf("unpack success\n");
	}

	return 0;
}
