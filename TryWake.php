<?php

$list = @$_REQUEST['List'];
$code = @$_REQUEST['Code'];
$class = @$_REQUEST['Class'];
$method = @$_REQUEST['Method'];

$root = '/var/wakelang';
$binroot = $root . '/wake-compiler/bin';

$command1 = '';
$command2 = '';
$compiler_output = '';

if($code) {
	putenv("LD_LIBRARY_PATH=" . getenv("LD_LIBRARY_PATH") . ":$root/boostpackage/libs");
	$filename = "$root/code/" . date('U') . rand();
	file_put_contents($filename, $code);
	$command1 = "$binroot/wake -d $binroot/waketable " . $filename . ' -o ' . $filename . '.o';
	$command1 .= ' 2>&1';

	$command2 = "$binroot/wake -d $binroot/waketable -l $binroot/wakeobj/std.o " . $filename . ".o -o $filename.js";
	if($list) $command2 .= ' -i';
	if($class) $command2 .= ' -c ' . escapeshellarg($class);
	if($method) $command2 .= ' -m ' . escapeshellarg($method);
	$command2 .= ' 2>&1';

	ob_start();
	system($command1);
	if(file_exists(@$filename . '.o'))
		system($command2);

	$compiler_output = trim(ob_get_clean());
}
?>
<html>
	<head>
		<title>Wake The Language</title>
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<!-- Bootstrap -->
		<link href="bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">
		<link rel="stylesheet" href="/highlight.js/styles/tomorrow.css">
		<script src="/highlight.js/highlight.pack.js"></script>
		<script src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>
		<script type="text/javascript" src="page.js"></script>
		<link rel="stylesheet" type="text/css" href="page.css">
		<link href='http://fonts.googleapis.com/css?family=Roboto:100' rel='stylesheet' type='text/css'>
	</head>
<body>
	<div class="mynavbar">
		<ul class="nav nav-tabs nav-stacked">
			<li><a href="index.html">Wake Home</a></li>
			<li><a href="faq.html">Wake FAQ</a></li>
			<li><a href="howto.html">Getting Started Guide</a></li>
			<li><a href="installing.html">Installing</a></li>
		</ul>
	</div>
	<div class="opennav"><button class="btn btn-inverse btn-small"><i class="icon-align-justify icon-white"></i></button></div>
		<div class="content" data-spy="scroll" data-target=".mynavbar">
			<div class="inner-content">
			<div id="intro">
				<div id="logo">
				</div>
				<h2><a class="logo-label" href="/">WAKE</a></h2>
				<p class="logo-label-label">
				a <span class="accent">fast</span>, <span class="accent">expressive</span>, <span class="accent">typesafe</span> language that gives you <span class="accent">testability from the ground up</span>.
				</p>
			</div>
		<h1>Online Wake to Javascript compiler</h1>
		<form method="POST">
		<div class="row">
			<div class="span12">
				<textarea style="width:100%; height:300px" name="Code"><?php if($code): echo $code; else: ?>
import Printer;

every Main is:

	needs Printer;

	main() {
		Printer.printLine('Hello World!');
	}
				<?php endif; ?></textarea>
			</div>
		</div>
		<div class="row">
			<div class="span6">
				Main class (defaults to Main):
			</div>
			<div class="span6">
				<input type="text" name="Class" value="<?php echo $class; ?>"/>
			</div>
		</div>
		<div class="row">
			<div class="span6">
				Main method (defaults to main):
			</div>
			<div class="span6">
				<input type="text" name="Method"value="<?php echo $method; ?>" />
			</div>
		</div>
		<div class="row"><div class="span12">OR</div></div>

		<div class="row">
			<div class="span6">List available main classes &amp; methods</div>
			<div class="span6"><input type="checkbox" name="List"></div>
		</div>
		<div class="row"><div class="span12">
			<input type="submit" value="Compile" />
		</div></div>
		<br /><br />
<?php if($compiler_output): ?>
		<h3>Compiler Errors:</h3>
		<pre><?php echo $compiler_output; ?></pre>
<?php endif; ?>

<?php if($code): ?>
	<?php if(file_exists(@$filename . '.js')): ?>
		<h3>Compilation Result:</h3>

		<pre><?php echo htmlentities(file_get_contents($filename . '.js')); ?></pre>
	<?php else: ?>
		<h3>No compilation result.</h3>
	<?php endif; ?>
<?php endif;?>
		</form>
		<?php if(file_exists(@$filename . '.js')): ?>
		<input type="button" onclick="eval(document.getElementById('result').value);" value="Eval it"/>

		<input type="hidden" id="result" value="<?php echo htmlentities(file_get_contents($filename . '.js')); ?>" />
		<?php endif; ?>
	</div>
</div>
</body>
</html>
