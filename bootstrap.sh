#!/bin/sh

git submodule update --init
svn export http://svn.boost.org/svn/boost/tags/release/Boost_1_45_0/boost
