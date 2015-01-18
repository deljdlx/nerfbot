

char * getFileContent(char * fileName) {
	char * buffer = 0;
	long length;
	FILE * f = fopen (fileName, "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = malloc (length);
	  if (buffer)
	  {
		fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}
	
	return buffer;
}


