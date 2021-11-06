#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
/*
—охран€ет изображение в файл .tga
path - путь к файлу
data - указатель на данные изображени€
w - ширина изоюражени€
h - высота изображени€
bpp - количество бит на пиксель
возвращает true, если удалось записать данный в файл, иначе false
*/
bool Array2Targa(const char* path,  const unsigned char* data,
	unsigned w, unsigned h, unsigned bpp)
{
	unsigned char TargaMagic[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	if (bpp == 8)
		TargaMagic[2] = 3;
	else
		TargaMagic[2] = 2;
	FILE* File = fopen(path,"wb");
	if (File == NULL)
    {
        return false;
    }
	if (fwrite(TargaMagic, 1, sizeof(TargaMagic), File) != sizeof(TargaMagic))
	{   
		fclose(File);
		return false;
	}
	unsigned char Header[6] = { 0 };
	Header[0] = w & 0xFF; Header[1] = (w >> 8) & 0xFF;
	Header[2] = h & 0xFF; Header[3] = (h >> 8) & 0xFF;
	Header[4] = bpp;
	unsigned int ImageSize = w*h*(bpp) / 8;
	if (fwrite(Header, 1, sizeof(Header), File) != sizeof(Header))
	{   
		fclose(File);
		return false;
	}
	if (fwrite(data, 1, ImageSize, File) != ImageSize)
	{   
		fclose(File);
		return false;
	}
	fclose(File);
	return true;
}

/*
«агружает изображение из несжатого .tga
path - путь к файлу
pdata - указатель на переменную, в которую будет записан указатель на считанные данные (очищать вручную)
pw - указатель на переменную, в которую будет записана ширина изображени€
ph - указатель на переменную, в которую будет записана выоста изображени€
pbpp - указатель на переменную, в которую будет записано количество бит на пиксель
возвращает true, если удалось считать данные из файла, иначе false
*/
bool Targa2Array(const char* path, unsigned char** pdata, unsigned* pw,
	unsigned* ph, unsigned* pbpp)
{
	const unsigned char TargaMagic[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	unsigned char FileMagic[12];
	unsigned char Header[6];
	FILE* File = fopen(path, "rb");
	if (File == NULL)
    {   
        std::cout<<"Can't open the file! Pointer = NULL\n";
		return false;
    }
	if (fread(FileMagic, 1, sizeof(FileMagic), File) != sizeof(FileMagic))
	{   
		fclose(File);
		return false;
	}
	unsigned char ImageType = FileMagic[2];
	FileMagic[2] = 0;
    //int a=0,b=0;
	if (memcmp(TargaMagic, FileMagic, sizeof(TargaMagic)) != 0
		|| fread(Header, 1, sizeof(Header), File) != sizeof(Header))
	{   
		fclose(File);
		return false;
	}
	//ќпредел€ем размеры изображени€
	*pw = Header[1] * 256 + Header[0];
	*ph = Header[3] * 256 + Header[2];
	//ќпредел€ем глубину цвета используемую в изображении
	*pbpp = Header[4];
	unsigned int Bpp = *pbpp / 8;
	//ѕоддерживаютс€ только изображени€ с 1,3 или 4 байта на пиксель
	if (*pw <= 0 || *ph <= 0 || (ImageType == 2 && Bpp != 3 && Bpp != 4) ||
		(ImageType == 3 && Bpp != 1))
	{   
		fclose(File);
		return false;
	}
	unsigned int ImageSize = *pw**ph*Bpp;
	unsigned char* data = (unsigned char*)malloc(ImageSize);
	//„итаем данные
	if (data == NULL || fread(data, 1, ImageSize, File) != ImageSize)
	{   
		free(data);
		fclose(File);
		return false;
	}
	//«акрываем файл
	fclose(File);
	*pdata = data;
	return true;
}