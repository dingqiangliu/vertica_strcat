<html lang="zn_CN"> <head> <meta charset='utf-8'> <title>Concat strings of multiple rows for Vertica</title> </head> <body>

Concat strings of multiple rows for Vertica
==========
This is a Vertica User Defined Functions (UDF) for string strcat function, just like Oracle's.

Syntax:
----------

STRCAT ( string [using parameters seperator=':separator', maxsize=:maxsize] ) over(...)

Parameters:

 * string: input string.
 * separator: separator string for concatenating, default value is ', '.
 * maxsize: maximum output size, default value is 64000. 
 * (return): concat string of input express on window. 

Examples:
----------

<code><pre>
	create table if not exists STRCATTEST.CITY(
	  COUNTRY VARCHAR2(20)
	  , CITY VARCHAR2(20)
	);
	truncate table STRCATTEST.CITY;
	insert into STRCATTEST.CITY values('China', 'Beijing');
	insert into STRCATTEST.CITY values('China', 'Hongkong');
	insert into STRCATTEST.CITY values('China', 'Taibei');
	insert into STRCATTEST.CITY values('Japan', 'Tokyo');
	insert into STRCATTEST.CITY values('Japan', 'Osaka');
	commit;

</code></pre>
<code><pre>
	select COUNTRY, strcat(CITY) over (partition by COUNTRY) as CITIES from STRCATTEST.CITY;
         COUNTRY |          CITIES           
        ---------+---------------------------
         China   | Beijing, Hongkong, Taibei
         Japan   | Osaka, Tokyo
        (2 rows)
</code></pre>
<code><pre>
	select COUNTRY, strcat(CITY using parameters separator =' & ', maxsize=7) over (partition by COUNTRY) as CITIES from STRCATTEST.CITY;
         COUNTRY |          CITIES           
        ---------+---------------------------
         China   | Beijing & ...
         Japan   | Osaka & ...
        (2 rows)
</code></pre>
<code><pre>
	select COUNTRY, strcat(CITY, upper(CITY)) over (partition by COUNTRY order by CITY desc) as (CITIES, CITIES_UPPER) from STRCATTEST.CITY;
         COUNTRY |          CITIES           |       CITIES_UPPER        
        ---------+---------------------------+---------------------------
         China   | Taibei, Hongkong, Beijing | TAIBEI, HONGKONG, BEIJING
         Japan   | Tokyo, Osaka              | TOKYO, OSAKA
        (2 rows)
</code></pre>


Install, test and uninstall:
----------
Befoe build and install, g++ should be available(yum -y groupinstall "Development tools" && yum -y groupinstall "Additional Development" can help on this).

 * Build: make
 * Install: make install
 * Test: make run
 * Uninstall make uninstall

</body> </html>



