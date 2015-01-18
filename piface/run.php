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

	protected $configurationFile='configuration.json';

	protected $devices=array();
	protected $drivers;
	protected $frequency=1;
	
	
	protected $sharedMemoryKey;
	protected $sharedMemorySize;
	protected $semaphoreKey;
	
	
	
	protected $switches=array();
	//protected $switchesValues=array();

	public function __construct() {
	
	
		$configuration=json_decode(file_get_contents(__DIR__.'/../configuration/'.$this->configurationFile));
		
		if(isset($configuration->sharedMemoryKey)) {
			$this->sharedMemoryKey=$configuration->sharedMemoryKey;
		}
		if(isset($configuration->sharedMemorySize)) {
			$this->sharedMemorySize=$configuration->sharedMemorySize;
		}
		if(isset($configuration->semaphoreKey)) {
			$this->semaphoreKey=$configuration->semaphoreKey;
		}
		
		
		/*
		$this->driver=PiFaceDigital::create();
		$this->driver->init();
		$switches=$this->driver->getSwitches();
		
		foreach($switches as $key=>$switch) {
			$this->switches[]=new GC_Switch($this, $switch);
		}
		*/
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

		$frequency=1000000/$this->frequency;


		$buffer=rand(0, 180)."\t".rand(0, 180)."\n";
		
		$buffer="0\t0\n";
		//$buffer="180\t180\n";
		
		echo $buffer;
		
		//echo "sem get\n";
		$this->memoryLock=sem_get($this->semaphoreKey, 1, 066, 1);
		
		//echo "sem acquire\n";
		$sem=sem_acquire($this->memoryLock);
		
		//echo "memory allocation\n";
		$shared_memory_id = shmop_open($this->sharedMemoryKey, "c", 0666, $this->sharedMemorySize);
		//echo "memory write\n";
		$result=shmop_write($shared_memory_id, $buffer, 8);			//Shared memory id, string to write, Index to start writing from


		//echo "sem release\n";
		sem_release($this->memoryLock);
	
		
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

/*
$bot->getComputer()->getSwitch(0)->onClick(function() {
	$this->getDevice(0)->setCycle(
		($this->getDevice(0)->getCycle()+1)%8+1
	);
});
*/

$bot->run();
exit();
