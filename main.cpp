//
//  main.cpp
//  N-d polygon
//
//  Created by Quinlan Bock (and Cody Nettesheim) on 6/29/18.
//  Copyright © 2018 Quinlan Bock. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

// Point data structure
struct Point
{
    int time;
    double latitude;
    double longitude;
    double altitude;
};

// Struct to return vector and size
struct vecSize{
    vector<Point> vec;
    int size;
};

// Method to find distance between two points
double distance(Point x, Point y){
    return sqrt(pow((y.latitude - x.latitude), 2.0) + pow((y.longitude - x.longitude), 2.0));
}

// Method to read in csv file
vecSize readFile(string path){
    vector<Point> coordinates(1000);
    
    // Read in the values from csv files
    std::ifstream in;
    in.open(path);
    if(!in.is_open()) std::cout << "ERROR: File '" + path + "' could not be opened" << '\n';
    
    int count = 0;
    
    string unixtime;
    string latitude;
    string longitude;
    string altitude;
    
    while(in.good()){
        getline(in,unixtime,',');
        getline(in,latitude,',');
        getline(in,longitude,',');
        getline(in,altitude,'\n');
        
        // Create point store converted strings as doubles
        Point p;
        
        p.time = atof(unixtime.c_str());
        p.latitude = atof(latitude.c_str());;
        p.longitude = atof(longitude.c_str());
        p.altitude = atof(altitude.c_str());
        coordinates[count] = p;
        
        count++;
    }
    in.close();
    
    vecSize toReturn;
    toReturn.vec = coordinates;
    toReturn.size = count;
    return toReturn;
}

// Adjust the unix timestamp so that that launch is 0
void adjust(vecSize & toAdjust){
    int x = toAdjust.vec[0].time;
    for(int i = 0; i<=toAdjust.size; i++){
        toAdjust.vec[i].time -= x;
    }
}

// Function to print out the data in the vector
void print(vecSize & toPrint){
    cout << "time, latitude, longitude, altitude \n";
    for(int i = 0; i<toPrint.size; i++){
        cout << toPrint.vec[i].time << "," << toPrint.vec[i].latitude << ","
        << toPrint.vec[i].longitude << "," << toPrint.vec[i].altitude <<"\n";
    }
}

// Helper function to find closest point based on timestamp
int findClosest(int timestamp, vecSize & act){
    int index = 0;
    //Loop through each timestamp and check if it is closer than the last
    for (int i = 1; i < act.size; i++) {
        int difference = abs(timestamp - act.vec[i].time);
        int lastDifference = abs(timestamp - act.vec[i - 1].time);
        if (difference < lastDifference) {
            index = i;
        }
    }
    return index;
}

//Helper function to find exact point based on timestamp
Point findPoint(int timestamp, vecSize & act) {
    Point point;
    for (int i = 0; i < act.size; i++) {
        Point current = act.vec[i];
        Point prev = act.vec[i - 1];
        //Return the point with the matching timestamp if it exists
        if (current.time == timestamp) {
            point = current;
            break;
        }
        //Otherwise, find the coordinates at the given timestamp based on the coordinates at the
        // 2 closest (before and after) timestamps
        else if (current.time > timestamp && prev.time < timestamp) {
            int deltaTime = current.time - prev.time;
            double deltaAlt = current.altitude - prev.altitude;
            double deltaLat = current.latitude - prev.latitude;
            double deltaLong = current.longitude - prev.longitude;
            point.time = timestamp;
            point.altitude = (deltaAlt / deltaTime) * (timestamp - prev.time);
            point.latitude = (deltaLat / deltaTime) * (timestamp - prev.time);
            point.longitude = (deltaLong / deltaTime) * (timestamp - prev.time);
            break;
        }
    }
    return point;
}

// Functions to go through and find the devations between given points
void calcDev(vector<double> & deviations, vecSize & pre,vecSize & act){
    // Loop through the predictions (They have a lower resolution than the actual coordinates)
    for(int i = 0; i<=pre.size; i++)
    {
        Point p = pre.vec[i];
        int pTime = p.time;
        Point a = findPoint(pTime, act);
        deviations[i] = distance(a,p);
    }
}

int main(int argc, const char * argv[]) {
    string prediction_file = "../prediction.csv";
    string actual_file = "../actual.csv";
    
    vecSize prediction = readFile(prediction_file);
    vecSize actual = readFile(actual_file);
    
    adjust(prediction);
    adjust(actual);
    
    // Confirm that everything has read in correctly
    cout << "Prediction:" << "\n";
    print(prediction);
    cout << "\n";
    cout << "Actual:" << "\n";
    print(actual);
    cout << "\n";
    
    vector<double> deviations(prediction.size);     // vector to hold the deviations
    calcDev(deviations,prediction,actual);

    cout << "Deviation:" << "\n";
    for(int i = 0; i<prediction.size; i++){
        cout << deviations[i] << " degrees" << "\n";
    }
}
