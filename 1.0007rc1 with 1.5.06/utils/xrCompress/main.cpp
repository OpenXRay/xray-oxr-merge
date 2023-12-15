int __cdecl main	(int argc, char* argv[])
{
	g_temporary_stuff	= &trivial_encryptor::decode;
	g_dummy_stuff		= &trivial_encryptor::encode;

	Core._initialize("xrCompress",0,FALSE);
	printf			("\n\n");

	LPCSTR params = GetCommandLine();

#ifndef MOD_COMPRESS
	if(strstr(params,"-store"))
	{
		bStoreFiles = TRUE;
	};
	{
		LPCSTR					temp = strstr(params,"-max_size");
		if (temp) {
			u64					test = u64(1024*1024)*u64(atoi(temp+9));
			if (u64(test) >= u64(u32(1) << 31))
				printf			("! too large max_size (%I64u), restoring previous (%I64u)\n",test,u64(XRP_MAX_SIZE));
			else
				XRP_MAX_SIZE	= u32(test);
		};
	}
#else
	bStoreFiles = TRUE;
#endif
#ifndef MOD_COMPRESS
	if(strstr(params,"-diff"))
	{
		ProcessDifference	();
	}else
#endif
	{
		#ifndef MOD_COMPRESS
		if (argc<2)	
		{
			printf("ERROR: u must pass folder name as parameter.\n");
			printf("-diff /? option to get information about creating difference.\n");
			printf("-fast	- fast compression.\n");
			printf("-store	- store files. No compression.\n");
			printf("-ltx <file_name.ltx> - pathes to compress.\n");
			printf("\n");
			printf("LTX format:\n");
			printf("	[config]\n");
			printf("	;<path>     = <recurse>\n");
			printf("	.\\         = false\n");
			printf("	textures    = true\n");
			
			Core._destroy();
			return 3;
		}
		#endif

		string_path		folder;		
		strconcat		(sizeof(folder),folder,argv[1],"\\");
		_strlwr_s		(folder,sizeof(folder));
		printf			("\nCompressing files (%s)...\n\n",folder);

		FS._initialize	(CLocatorAPI::flTargetFolderOnly|CLocatorAPI::flScanAppRoot,folder);

		BOOL bFast		= 0!=strstr(params,"-fast");

		LPCSTR p		= strstr(params,"-ltx");
#ifndef MOD_COMPRESS
		if(0!=p)
		{
			ProcessLTX		(argv[1],p+4,bFast);
		}else{
			ProcessNormal	(argv[1],bFast);
		}
#else
		R_ASSERT2		(p, "wrong params passed. -ltx option needed");
		ProcessLTX		(argv[1],p+4,bFast);
#endif
	}

	Core._destroy		();
	return 0;
}
