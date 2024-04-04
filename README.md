# Water Supply Management Analysis Tool

This project is a part of the Design of Algorithms course (DA) in Spring 2024.

## Introduction
The Water Supply Management Analysis Tool is developed to assist the Management Team of a water supply network in Portugal in making informed decisions about resource allocation. The tool allows for the analysis of network metrics, evaluation of network resilience, and identification of potential improvements.

## Problem Statement
The project aims to implement an analysis tool that supports the Management Team of a water supply network in making informed decisions about resource allocation, including financial and physical resources such as pumping stations and pipelines. The tool should allow management to allocate pumping stations to supply sources effectively, identify sensitive network sections, and mitigate service disruptions.

## Running
To run the Water Supply Management Analysis Tool, please follow these steps:
```bash
mkdir build
cd build
cmake ..
make
./DA2324_PRJ1_G184
```

## Usage
The tool provides the following functionalities:
- Calculation of maximum flow reaching each city in the network.
- Evaluation of water deficit or excess for each city.
- Analysis of the impact of reservoir, pumping station, and pipeline failures on network reliability.
- Balancing the load across the network to minimize flow differences.

#### Authors
Amanda Tartarotti<br/>
Marcel Medeiros<br/>
Pedro Castro<br/>
