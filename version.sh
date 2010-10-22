#!/bin/bash

let version=$(tail -n 8 inc/version.h | head -n 1 | cut -f 3 -d' ')+1
branch=$(git branch | grep \* | sed s/\*\ //g)

cat << EOF > inc/version.h
/*
 *  ASXSoft Nuke - Operating System - kernel5 - Codename: 理コ込
 *  Copyright (C) 2010 Patrick Pokatilo
 *
 *  This file is part of Nuke (理コ込).
 *
 *  Nuke (理コ込) is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nuke (理コ込) is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Nuke (理コ込).  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _VERSION_H_
#define _VERSION_H_

EOF

echo "#define __BUILD__ $version" >> inc/version.h
echo "#define __COMMIT__ $(git rev-list $branch | wc -l)" >> inc/version.h
echo "#define __HEAD__ \"$(git rev-list $branch | head -n 1)\"" >> inc/version.h
echo "#define __BRANCH__ \"$branch\"" >> inc/version.h
echo "#define __CHANGED__ \"$(git log -n 1 | grep Date | sed 's/Date:[\t\ ]*//g')\"" >> inc/version.h
echo "#define __TAG__ \"$(git tag | tail -n 1)\"" >> inc/version.h
echo >> inc/version.h
echo "#endif" >> inc/version.h

git add inc/version.h
