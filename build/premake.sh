#!/bin/sh
premake5 gmake
sed -i '' 's/,-x//' gmake/*.make