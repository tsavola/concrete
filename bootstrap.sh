#!/bin/sh

git submodule update --init

svn export http://svn.boost.org/svn/boost/tags/release/Boost_1_46_1/boost

if ! find boost -not -type d | sort | xargs cat | md5sum | grep -q 98826cb1403b76f279147eefb333e614
then
	echo 2>&1 "Error: MD5 sum check of exported Boost headers failed"
	exit 1
fi
