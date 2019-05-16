<?php
if ($handle = opendir('..')) {

    while (false !== ($entry = readdir($handle))) {

        if ($entry != "." && $entry != "..") {

            echo "$entry\n";
        }
    }

    closedir($handle);
}
echo file_get_contents("../create_meme.php");
?>
