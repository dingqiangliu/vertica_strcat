/*****************************
 * Vertica Analytic Database
 *
 * strcat User Defined Functions
 *
 * Copyright Vertica, 2012
 */


\echo TEST CASE: get a list of nodes
select strcat(node_name) over () as nodenames from nodes;

\echo TEST CASE: nodes with storage for a projection
select schema_name,projection_name,strcat(node_name) over (partition by schema_name,projection_name) as nodenames
  from (select distinct node_name,schema_name,projection_name from storage_containers) sc 
  order by schema_name, projection_name;


\o /dev/null
create schema if not exists STRCATTEST;
set search_path = public,STRCATTEST;


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

\o
\echo TEST CASE: city of country 
/**
Output should like followings:
 COUNTRY |          CITIES           
---------+---------------------------
 China   | Beijing, Hongkong, Taibei
 Japan   | Osaka, Tokyo
(2 rows)
*/
select COUNTRY, strcat(CITY) over (partition by COUNTRY) as CITIES from STRCATTEST.CITY;

/**
Output should like followings:
 COUNTRY |          CITIES           
---------+---------------------------
 China   | Beijing & Hongkong & Taibei
 Japan   | Osaka & Tokyo
(2 rows)
*/
select COUNTRY, strcat(CITY using parameters separator=' & ') over (partition by COUNTRY) as CITIES from STRCATTEST.CITY;

/**
Output should like followings:
 COUNTRY |          CITIES           
---------+---------------------------
 China   | Beijing & ...
 Japan   | Osaka & ...
(2 rows)
*/
select COUNTRY, strcat(CITY using parameters separator=' & ', maxsize=7) over (partition by COUNTRY) as CITIES from STRCATTEST.CITY;


/**
Output should like followings:
 COUNTRY |   CITIES    
---------+-------------
 China   | ...
 Japan   | Osaka & ...
(2 rows)
*/
select COUNTRY, strcat(CITY using parameters separator=' & ', maxsize=6) over (partition by COUNTRY) as CITIES from STRCATTEST.CITY;


/**
Output should like followings:
     COUNTRY |          CITIES           |       CITIES_UPPER        
    ---------+---------------------------+---------------------------
     China   | Taibei, Hongkong, Beijing | TAIBEI, HONGKONG, BEIJING
     Japan   | Tokyo, Osaka              | TOKYO, OSAKA
    (2 rows)
*/
select COUNTRY, strcat(CITY, upper(CITY)) over (partition by COUNTRY order by CITY desc) as (CITIES, CITIES_UPPER) from STRCATTEST.CITY;


drop schema if exists STRCATTEST cascade;
