<?php
use Pkj\Raspberry\PiFace\PiFaceDigital;

//dl("spi.so"); // Load the SPI extension.
require 'vendor/autoload.php';

$dev = PiFaceDigital::create();
// Run once.
$dev->init();



$pins=$dev->getOutputPins();

$t=500;

while(1) {
	$pins[7]->setValue(1);
	usleep($t);
	$pins[7]->setValue(0);
	usleep($t);
}



exit();

while(1) {
for($i=0; $i<7; $i++) {
	//echo $dev->getInputPins()[3]->getValue()." ";

	$dev->getOutputPins()[$i]->setValue(1);
	
	//exit();

	for($j=0;$j<7; $j++) {
		echo $dev->getOutputPins()[$j]->getValue();
	}
	
	echo "\t";
	
	for($j=0;$j<4; $j++) {
		echo $dev->getSwitches()[$j]->getValue();
	}

	
	echo "\n";


	//$dev->getLeds()[$i]->turnOn();
	//$dev->getOutputPins()[$i]->setValue(1);

	//usleep(1000);

	sleep(1);


	//usleep(10000);
	//$dev->getLeds()[$i]->turnOff();

	$dev->getOutputPins()[$i]->setValue(0);
}
}


// $dev->getInputPins();
// $dev->getOutputPins();
// $dev->getLeds();
// $dev->getRelays();
// $dev->getSwitches();


// Turn on relay 0
//$dev->getRelays()[0]->turnOn();

// Get 0/1 of input pin 3 (There are 8 pins, 0-7)
//$dev->getInputPins()[3]->getValue();

// Toggle a value on a output pin (5 in this example)
//$dev->getOutputPins()[5]->toggle(); // 0
//$dev->getOutputPins()[5]->toggle(); // 1
//$dev->getOutputPins()[5]->toggle(); // 0
