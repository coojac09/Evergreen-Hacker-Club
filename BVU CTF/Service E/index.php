<html>
<head>
<title>Meme Generator</title>
</head>
<body>

<center><table><tr><td><img src="blb.jpg"></td><td valign=center>


<div style="border-radius: 20px 20px 20px 20px; background-color: #AAAAAA;">
<h2 style="text-align:center">Create your own meme!</h2>

<form action="create_meme.php" method="post" enctype="multipart/form-data">
<table>
  <tr><td align=right>Filename:</td><td><input type="file" name="imageToUpload"></td></tr>
  <tr><td align=right>Top Text:</td><td><input type="text" name="topText" size="40"></td></tr>
  <tr><td align=right>Bottom Text:</td><td><input type="text" name="botText" size="40"></td></tr>
  <tr><td colspan=2 align=center><input type="submit" name="submit" value="Submit"></td></tr>
</table>
</form>
</div>

</td></tr></table></center>

</body>
</html>
