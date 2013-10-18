#!/bin/bash
##############################################################################
# Copyright (c) 2013, Robert Wloch
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
# Robert Wloch - initial API and implementation
##############################################################################

if [ ! $# -eq 1 ]; then
  echo "usage: ${0} \"<file-pattern>\""
  exit 0
fi

lcov -d . -c -o coverage.info

lcov --list-full-path -e coverage.info ${1} -o coverage-stripped.info

genhtml -o html-coverage coverage-stripped.info

lcov -d . -z

exit 0

