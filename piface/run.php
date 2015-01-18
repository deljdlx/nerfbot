<?php
use Pkj\Raspberry\PiFace\PiFaceDigital;

//dl("spi.so"); // Load the SPI extension.
require 'vendor/autoload.php';

$dev = PiFaceDigital::create();
// Run once.
$dev->init();


class GC_Bot
{

	

	public function __construct() {
		$this->computer=new Raspberry();
		
		$this->xServo=new GC_Servo();
		$this->yServo=new GC_Servo();
		
		
		$this->computer->registerDevice($this->xServo);
		$this->computer->registerDevice($this->yServo);
		
	}
	
	
	public function getComputer() {
		return $this->computer;
	}
	
	public function run() {
		$this->computer->run();
	}
}



class GC_Servo
{
	
	protected $tick=0;
	protected $tickCycle=1;
	protected $value=0;

	public function __construct() {
		//$this->tickCycle=rand(1,1);

		$this->tickCycle=2;
	}

	public function setCycle($value) {
		$this->tickCycle=$value;
	}

	public function rotate($angle) {
		$this->tickCycle=4;
	}
	
	public function getCycle() {
		return $this->tickCycle;
	}
	

	
	public function tick() {
		$this->tick=($this->tick+1)%$this->tickCycle;
		if($this->tick==1) {
			$this->value=0;
		}
		else {
			$this->value=1;
		}
		
		//echo $this->value."\n";
	}
	
	public function getValue() {
		return $this->value;
	}
}


class GC_Switch
{
	protected $lastState=0;
	protected $driver;
	protected $onClick=null;
	
	protected $computer;
	
	
	public function __construct($computer, $driver) {
		$this->driver=$driver;
		$this->computer=$computer;
	}

	public function getValue() {
		return $this->driver->getValue();
	}

	public function onClick($callback=null) {
		if($callback) {
			$this->onClick=$callback;
		}
		else if($this->onClick){
			call_user_func_array(array($this->onClick->bindTo($this->computer), '__invoke'), array());
		}
	}
}


class Raspberry
{

	protected $devices=array();
	protected $drivers;
	protected $frequency=1;
	
	protected $switches=array();
	//protected $switchesValues=array();

	public function __construct() {
		$this->driver=PiFaceDigital::create();
		$this->driver->init();

		$switches=$this->driver->getSwitches();
		
		foreach($switches as $key=>$switch) {
			$this->switches[]=new GC_Switch($this, $switch);
		}
	}
	


	public function getDevice($index) {
		if(isset($this->devices[$index])) {
			return $this->devices[$index];
		}
		else {
			return false;
		}
	}
	

	public function registerDevice($device) {
		$this->devices[]=$device;
	}
	
	public function sendOutput() {

		//echo 

	
		//$pins=$this->driver->getOutputPins();
	
		foreach($this->devices as $pin=>$device) {
			echo $device->getValue()."\t";
			$index=$pin+2;
			$device->tick();
			/*
			$pins[$index]->setValue(
				!$device->getValue()
			);
			*/
		}
		echo "\n";
	}
	
	public function loadSwitchValues() {
		foreach($this->switches as $index=>$switch) {
			$newValue=$switch->getValue();
		
			if(!isset($this->switchesValues[$index])) {
				$this->switchesValues[$index]=0;
			}
			
			
			if($newValue!=$this->switchesValues[$index] && $newValue) {
				$switch->onClick();
			}
		
			$this->switchesValues[$index]=$newValue;
		}
	}
	
	public function getSwitch($index) {
		if(isset($this->switches[$index])) {
			return $this->switches[$index];
		}
	}
	
	public function getSwitchValue($index) {
		return $this->switchesValues[$index];
	}
	
	
	public function run() {
	
		$key=672213396;
		
		
		//$key = ftok("/tmp/shared", 'R');
	
		$shared_memory_id = shmop_open($key, "c", 0666, 1024);
	
	
	
		//echo shmop_read($shared_memory_id, 0, 1024);
		//die();
	
		$frequency=1000000/$this->frequency;
		
		//echo $frequency;
		
		
		
		$shared_memory_string='hello world';



		//CONVERT TO AN ARRAY OF BYTE VALUES
		$shared_memory_array = array_slice(unpack('C*', "\0".$shared_memory_string), 1);

		//echo "Shared memory bytes: ";
		for($i = 0; $i < 10; $i++)
		{
			//echo $shared_memory_array[$i] . ", ";
		}
		//echo "<br />";



					//The array to write
			$shared_memory_array = array(30, 255);
			
			//Convert the array of byte values to a byte string
			$shared_memory_string = call_user_func_array('pack', array_merge(array("C*"), $shared_memory_array));
			
			//echo "Writing bytes: $shared_memory_string\n";
			
			
			$buffer=rand(0, 180)."\t".rand(0, 180)."\n";
			echo $buffer;
			$result=shmop_write($shared_memory_id, $buffer, 8);			//Shared memory id, string to write, Index to start writing from
		
		return;




	
		while(1) {
		
			echo "90"."\t"."45"."\n";
		
			//$this->loadSwitchValues();
			//$this->sendOutput();
			
			
			$sleep=floor($frequency);
			usleep($sleep);
			
			/*
			$pins=$this->driver->getOutputPins();
			if($value=$pins[7]->getValue()) {
				$this->driver->getOutputPins()[7]->setValue(0);
			}
			else {
				$this->driver->getOutputPins()[7]->setValue(1);
			}
			*/
		}
	}
}

$bot=new GC_Bot();


$bot->getComputer()->getSwitch(0)->onClick(function() {
	
	//print_r($this->computer->devices);

	$this->getDevice(0)->setCycle(
		($this->getDevice(0)->getCycle()+1)%8+1
	);
});

$bot->run();
exit();







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
