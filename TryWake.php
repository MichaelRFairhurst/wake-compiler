<?php

$list = @$_REQUEST['List'];
$code = @$_REQUEST['Code'];
$class = @$_REQUEST['Class'];
$method = @$_REQUEST['Method'];

$root = '/var/wakelang';
$binroot = $root . '/wake-compiler/bin';

$command1 = '';
$command2 = '';
$output = '';

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
}

?>
<html>
<body>
<h1>Online Wake to Javascript compiler</h1>
<form method="POST">
<textarea name="Code" cols="120" rows="15"><?php if($code): echo $code; else: ?>
import Printer;

every Main is:

	needs Printer;

	main() {
		Printer.print('Hello World!');
	}
<?php endif; ?></textarea>
<br />
Main class (defaults to Main): <input type="text" name="Class" value="<?php echo $class; ?>"/> <br/>
Main method (defaults to main): <input type="text" name="Method"value="<?php echo $method; ?>" /> <br/>
OR<br/>
<input type="checkbox" name="List"> List available main classes &amp; methods
<br />
<input type="submit" value="Compile" />
<br /><br />
<?php if($code): ?>
<pre>Compiler Output:

<?php system($command1); ?><br/>
<?php if(file_exists(@$filename . '.o')): ?>
<?php system($command2); ?><br/>
<?php endif; ?>

<?php if(file_exists(@$filename . '.js')): ?>
Compilation Result:

<?php echo file_get_contents($filename . '.js'); ?>
<?php else: ?>
No compilation result.
<?php endif; ?>
</pre>
<?php endif; ?>
</form>
<?php if(file_exists(@$filename . '.js')): ?>
<input type="button" onclick="eval(document.getElementById('result').value);" value="Eval it"/>

<input type="hidden" id="result" value="<?php echo htmlentities(file_get_contents($filename . '.js')); ?>" />
<?php endif; ?>
</body>
</html>
