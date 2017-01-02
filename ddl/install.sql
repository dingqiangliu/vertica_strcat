/*****************************
 * Vertica Analytic Database
 *
 * strcat User Defined Functions
 *
 * Copyright Vertica, 2012
 */

-- Step 1: Create LIBRARY 
\set libfile '\''`pwd`'/lib/strcat.so\'';
CREATE LIBRARY strcat AS :libfile;

-- Step 2: Create cube/rollup Factory
\set tmpfile '/tmp/strcatinstall.sql'
\! cat /dev/null > :tmpfile

\t
\o :tmpfile
select 'CREATE TRANSFORM FUNCTION strcat AS LANGUAGE ''C++'' NAME '''||obj_name||''' LIBRARY strcat;' from user_library_manifest where lib_name='strcat' and obj_name like 'StrCatFactory%';

\o
\t

\i :tmpfile
\! rm -f :tmpfile
