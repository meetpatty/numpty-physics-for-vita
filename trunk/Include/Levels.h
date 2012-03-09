/*
 * This file is part of NumptyPhysics
 * Copyright (C) 2008 Tim Edmonds
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */
/*
* PSP port by rock88: rock88a@gmail.com
* http://rock88dev.blogspot.com
*/

#ifndef LEVELS_H
#define LEVELS_H

#include <cstdio>
#include <sstream>

#include "Array.h"
#include "zipfile.h"

class Levels
{

public:
	Levels(int numDirs=0, const char** dirs=NULL);
	bool addPath(const char* path);
	bool addLevel(const std::string& file, int rank=-1);
	int  numLevels();
	const std::string& levelFile(int i); 
	int  levelSize(int l);
	bool load(int l, void* buf, int buflen);
	int findLevel(const char *file);

private:
	bool scanCollection(std::string& file, int rank);
	
	struct LevelDesc
	{
		LevelDesc(const std::string& f,int r=0,int i=-1):file(f),index(i),rank(r)
		{
		}
		std::string file;
		int         index;
		int         rank;
	};
	
	Array<LevelDesc*> m_levels;

};

#endif //LEVELS_H
