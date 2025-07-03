## [no name yet]

### Purpose

the program is like EasyCharge it Management app for charge port around the country

### Key Components

- Station: a repository of charging stations, contains:
- - id: unique whole number
- - name: pointer to a dynamic string
- - nPorts: number of ports in the station
- - coord: coordinates the location of the station Struct Coord with fields:{double x,double y}
- - portsList: head of linked list of all the Ports on the station
- - nCars: number of cars on queue
- - carQueue: ADT of queue from type qCar
- - right: pointer to right station
- - left: pointer to left station

- Port: a repository of charging ports, contains:
- - num: number of the port whole number
- - portType: enum I created on Utilis called PortType
- - status: enum I created on Utilis called PoolStatus
- - p2car: pointer to Car who currently charge on the port if empty will be NULL
- - tin: time from the beginning of charge from type Date I created on Utilis
- - next: pointer to next port on list

- Car: a repository of clients of the company organized by car license
- - nLicense: unique number of car license string of 9 char will contain only whole number
- - portType: enum I created on Utilis called PortType
- - totalPayed: double ; total payed until today
- - qPort: pointer to Port when charge if no on charge NULL
- - inqueue: enum I created on Utilis called BOOL, FALSE=0 TRUE=1

- tCar: structure of a leaf in a BTS of cars:
- - data: pointer to Car
- - right: pointer to left Car
- - left: pointer to right Car

- carNode: structure of a node in a BTS of cars:
- - data: pointer to Car
- - next: pointer to next Car on queue

- qCar: ADT of cars queue:
- - front: pointer to head of queue
- - rear: pointer to tail of queue

- Date:
- - year: whole positive number max 4 char
- - month:number between 0-12
- - day: a whole positive number
- - hour: 24 hours format number 0-23
- - min: number 0-59

- progress: when the user open the program he will iteract with menu on consloe, the app includes smaller bugs right now

- Text files:
  i have text files where i take and manipulate the data
  Cars.txt:
  example-
  License,PortType,TotalPayed,StationID,PortNumber,InQueue
  25497284,SLOW,36.95,0,0,0
  22112748,SLOW,147.73,0,0,0

  Ports.txt:
  example-
  StationID,PortNumber,PortType,Status,Year,Month,Day,Hour,Min,CarLicense
  101,1,FAST,1,2025,6,30,14,30,12345678
  101,2,MID,1,2025,6,30,14,32,87654321

  LineOfCars.txt:
  example-
  License,StationID
  27205295,101
  24799602,101

  Stations.txt:
  example-
  ID,StationName,NumOfPorts,CoordX,CoordY
  101,Tel Aviv Center,4,32.09,34.78
  205,Haifa Port,3,32.79,34.99
