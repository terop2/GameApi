<?php
ini_set('output_buffering', 'Off');
use function OutputControl\ob_start;
use function OutputControl\ob_end_flush;
use function OutputControl\ob_get_clean;

@ob_end_clean();

header("Content-Type: image/jpg");

$file = file_get_contents("/home/terop/cvs/fastflux/test.jpg");
echo "$file";
    flush();
