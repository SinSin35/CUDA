#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
/*
��������� ����������� � ���� .tga
path - ���� � �����
data - ��������� �� ������ �����������
w - ������ �����������
h - ������ �����������
bpp - ���������� ��� �� �������
���������� true, ���� ������� �������� ������ � ����, ����� false
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
��������� ����������� �� ��������� .tga
path - ���� � �����
pdata - ��������� �� ����������, � ������� ����� ������� ��������� �� ��������� ������ (������� �������)
pw - ��������� �� ����������, � ������� ����� �������� ������ �����������
ph - ��������� �� ����������, � ������� ����� �������� ������ �����������
pbpp - ��������� �� ����������, � ������� ����� �������� ���������� ��� �� �������
���������� true, ���� ������� ������� ������ �� �����, ����� false
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
	//���������� ������� �����������
	*pw = Header[1] * 256 + Header[0];
	*ph = Header[3] * 256 + Header[2];
	//���������� ������� ����� ������������ � �����������
	*pbpp = Header[4];
	unsigned int Bpp = *pbpp / 8;
	//�������������� ������ ����������� � 1,3 ��� 4 ����� �� �������
	if (*pw <= 0 || *ph <= 0 || (ImageType == 2 && Bpp != 3 && Bpp != 4) ||
		(ImageType == 3 && Bpp != 1))
	{   
		fclose(File);
		return false;
	}
	unsigned int ImageSize = *pw**ph*Bpp;
	unsigned char* data = (unsigned char*)malloc(ImageSize);
	//������ ������
	if (data == NULL || fread(data, 1, ImageSize, File) != ImageSize)
	{   
		free(data);
		fclose(File);
		return false;
	}
	//��������� ����
	fclose(File);
	*pdata = data;
	return true;
}