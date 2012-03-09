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


struct zip_lfh {
	int ziplocsig;
	short zipver;
	short zipgenfld;
	short zipmthd;
	short ziptime;
	short zipdate;
	int zipcrc;
	unsigned int zipsize;
	unsigned int zipuncmp;
	unsigned short zipfnln;
	unsigned short zipxtraln;
	char zipname[0];
} __attribute__ ((packed));

struct zip_cd {
	int zipcensig;
	char zipcver;
	char zipcos;
	char zipcvxt;
	char zipcexos;
	short zipcflg;
	short zipcmthd;
	short ziptim;
	short zipdat;
	int zipccrc;
	unsigned int zipcsiz;
	unsigned int zipcunc;
	unsigned short zipcfnl;
	unsigned short zipcxtl;
	unsigned short zipccml;
	unsigned short zipdsk;
	unsigned short zipint;
	unsigned int zipext;
	unsigned int zipofst;
	char zipcfn[0];	
} __attribute__ ((packed));

struct zip_eoc {
	int zipesig;
	unsigned short zipedsk;
	unsigned short zipecen;
	unsigned short zipenum;
	unsigned short zipecenn;
	unsigned int zipecsz;
	unsigned int zipeofst;
	short zipecoml;
	char zipecom[0];
} __attribute__ ((packed));
