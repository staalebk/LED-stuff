<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name="viewport" content="initial-scale=1, user-scalable=no" />
<title>Disco ledz!</title>

<style>

#dark{
	background-color:#333;
	padding:0px;
  margin-top:20px;}
	
#light{
	background-color:#FFF;
	border:1px solid #dedede;
	padding:10px;
	margin-top:20px;}	

iframe{
  width:100%;
  height: 50px;
  border: 1px solid black;
}
ul{
  padding:0px
} 
li{ 
list-style:none;
	padding-top:10px;
	padding-bottom:10px;}	

.button, .button:visited {
	background: #222 url(overlay.png) repeat-x; 
  display: block; 
	padding: 5px 10px 6px; 
  color: #fff; 
	text-decoration: none;
	-moz-border-radius: 6px; 
	-webkit-border-radius: 6px;
	-moz-box-shadow: 0 1px 3px rgba(0,0,0,0.6);
	-webkit-box-shadow: 0 1px 3px rgba(0,0,0,0.6);
	text-shadow: 0 -1px 1px rgba(0,0,0,0.25);
	border-bottom: 1px solid rgba(0,0,0,0.25);
	position: relative;
	cursor: pointer
}
 
	.button:hover							{ background-color: #111; color: #fff; }
	.button:active							{ top: 1px; }
	.small.button, .small.button:visited 			{ font-size: 11px}
	.button, .button:visited,
	.medium.button, .medium.button:visited 		{ font-size: 13px; 
												  font-weight: bold; 
												  line-height: 1; 
												  text-shadow: 0 -1px 1px rgba(0,0,0,0.25); 
												  }
												  
	.large.button, .large.button:visited 			{ font-size: 14px; 
													  padding: 8px 14px 9px; }
													  
	.super.button, .super.button:visited 			{ font-size: 34px; 
													  padding: 8px 14px 9px; }
	
	.pink.button, .magenta.button:visited		{ background-color: #e22092; }
	.pink.button:hover							{ background-color: #c81e82; }
	.green.button, .green.button:visited		{ background-color: #91bd09; }
	.green.button:hover						    { background-color: #749a02; }
	.red.button, .red.button:visited			{ background-color: #e62727; }
	.red.button:hover							{ background-color: #cf2525; }
	.orange.button, .orange.button:visited		{ background-color: #ff5c00; }
	.orange.button:hover						{ background-color: #d45500; }
	.blue.button, .blue.button:visited		    { background-color: #2981e4; }
	.blue.button:hover							{ background-color: #2575cf; }
	.yellow.button, .yellow.button:visited		{ background-color: #ffb515; }
	.yellow.button:hover						{ background-color: #fc9200; }
		</style> 



</head> 
	<body bgcolor="#333"> 

  <?php
if (isset($_GET["a"])) {
  $anim = intval($_GET["a"]);
  print("Animation $anim!");
}
?> 
    <iframe name="curanim" src="anim.php"></iframe>
    <div id="dark">       
      <ul>
        <li>
          <a class="super button green" target="curanim" href="anim.php?a=1">Effect 1</a>
        </li>
        <li>
          <a class="super button pink" target="curanim" href="anim.php?a=2">Effect 2</a>
        </li>
        <li>
          <a class="super button blue" target="curanim" href="anim.php?a=3">Effect 3</a>
        </li>
        <li>
          <a class="super button orange" target="curanim" href="anim.php?a=4">Effect 4</a>
        </li>
        <li>
          <a class="super button green" target="curanim" href="anim.php?a=5">Effect 5</a>
        </li>
        <li>
          <a class="super button pink" target="curanim" href="anim.php?a=6">Effect 6</a>
        </li>
        <li>
          <a class="super button blue" target="curanim" href="anim.php?a=7">Effect 7</a>
        </li>
        <li>
          <a class="super button orange" target="curanim" href="anim.php?a=8">Effect 8</a>
        </li>
<!--
        <li>
          <a class="super button yellow" target="curanim" href="anim.php?a=100">Random</a>
        </li>
-->
      </ul>
    </div>     
	</body> 
</html> 
