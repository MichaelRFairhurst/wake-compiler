<?php

$list = @$_REQUEST['List'];
$code = @$_REQUEST['Code'];
$class = @$_REQUEST['Class'];
$method = @$_REQUEST['Method'];

$command = '';

if($code) {
	$filename = '/var/wakelang/code/' . date('U') . rand();
	file_put_contents($filename, $code);
	$command = '/var/wakelang/wake-compiler/bin/wake ' . $filename . ' -o ' . $filename . '.js';
	if($list) $command .= ' -l';
	if($class) $command .= ' -c ' . escapeshellarg($class);
	if($method) $command .= ' -m ' . escapeshellarg($method);
	$command .= ' 2>&1';
}

?>
<html>
<body>
<h1>Online Wake to Javascript compiler</h1>
<form>
<textarea name="Code" cols="120" rows="15"><?php if($code): echo $code; else: ?>
every Main is:

	needs Printer;

	Main() {
		Printer.print('Hello World!');
	}
<?php endif; ?></textarea>
<br />
Main class (defaults to Main): <input type="text" name="Class" value="<?php echo $class; ?>"/> <br/>
Main method (defaults to Main): <input type="text" name="Method"value="<?php echo $method; ?>" /> <br/>
OR<br/>
<input type="checkbox" name="List"> List available main classes &amp; methods
<br />
<input type="submit" value="Compile" />
<br /><br />
<?php if($code): ?>
<pre>Compiler Output:

<?php system($command);?>


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
