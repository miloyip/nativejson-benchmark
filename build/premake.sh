#!/bin/sh
premake5 gmake
sed -i.bak 's/,-x//' gmake/*.make