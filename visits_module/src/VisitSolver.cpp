/*
 <one line to give the program's name and a brief idea of what it does.>
 Copyright (C) 2015  <copyright holder> <email>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "VisitSolver.h"
#include "ExternalSolver.h"
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "armadillo"
#include <initializer_list>

using namespace std;
using namespace arma;

extern "C" ExternalSolver *create_object()
{
  return new VisitSolver();
}

extern "C" void destroy_object(ExternalSolver *externalSolver)
{
  delete externalSolver;
}

VisitSolver::VisitSolver()
{
}

VisitSolver::~VisitSolver()
{
}

void VisitSolver::loadSolver(string *parameters, int n)
{
  starting_position = "r0";
  string Paramers = parameters[0];

  char const *x[] = {"dummy"};
  char const *y[] = {"act-cost", "triggered"};
  parseParameters(Paramers);
  affected = list<string>(x, x + 1);
  dependencies = list<string>(y, y + 2);

  string waypoint_file = "visits_domain/waypoint.txt"; // change this to the correct path
  parseWaypoint_conn(waypoint_file);

  string landmark_file = "visits_domain/landmark.txt"; // change this to the correct path
  parseLandmark(landmark_file);

  string region_file = "visits_domain/region.txt";
  parseRegions(region_file);
}

map<string, double> VisitSolver::callExternalSolver(map<string, double> initialState, bool isHeuristic)
{
  map<string, double> toReturn;
  map<string, double>::iterator iSIt = initialState.begin();
  map<string, double>::iterator isEnd = initialState.end();
  double dummy;
  double act_cost;

  map<string, double> trigger;

  for (; iSIt != isEnd; ++iSIt)
  {
    string parameter = iSIt->first;
    string function = iSIt->first;
    double value = iSIt->second;

    function.erase(0, 1);
    function.erase(function.length() - 1, function.length());
    int n = function.find(" ");

    if (n != -1)
    {
      string arg = function;
      string tmp = function.substr(n + 1, 5);

      function.erase(n, function.length() - 1);
      arg.erase(0, n + 1);
      if (function == "triggered")
      {
        trigger[arg] = value > 0 ? 1 : 0;
        if (value > 0)
        {

          string from = tmp.substr(0, 2);
          string to = tmp.substr(3, 2);
          find_path(from, to);
        }
      }
    }
    else
    {
      if (function == "dummy")
      {
        dummy = value;
      }
      else if (function == "act-cost")
      {
        act_cost = value;
      }
    }
  }

  double results = calculateExtern(dummy, act_cost);

  if (ExternalSolver::verbose)
  {
    cout << "(dummy) " << results << endl;
  }

  toReturn["(dummy)"] = results;

  return toReturn;
}

list<string> VisitSolver::getParameters()
{
  return affected;
}

list<string> VisitSolver::getDependencies()
{
  return dependencies;
}

void VisitSolver::parseParameters(string parameters)
{
  int curr, next;
  string line;
  ifstream parametersFile(parameters.c_str());
  if (parametersFile.is_open())
  {
    while (getline(parametersFile, line))
    {
      curr = line.find(" ");
      string region_name = line.substr(0, curr).c_str();
      curr = curr + 1;
      while (true)
      {
        next = line.find(" ", curr);
        region_mapping[region_name].push_back(line.substr(curr, next - curr).c_str());
        if (next == -1)
          break;
        curr = next + 1;
      }
    }
  }
}

double VisitSolver::calculateExtern(double external, double total_cost)
{
  return cost;
}

void VisitSolver::parseWaypoint_conn(string waypoint_file)
{
  // Let's define the structure of the waypoints in 'waypoint.txt' file as follows: wp0[0,0,0,wp*,wp*,wp*,wp*,wp*,wp*].
  // It is essential that every waypoint is connected to a number from 3 to 6 other waypoints.
  int curr, next;
  string line;
  double pose1, pose2, pose3;
  int flag;
  string wp;
  vector<string> waypoint_connected;
  ifstream parametersFile(waypoint_file.c_str());
  if (parametersFile.is_open())
  {
    while (getline(parametersFile, line))
    {
      flag = 0;
      waypoint_connected.clear(); // when initialized, clear the vector

      curr = line.find("[");
      string waypoint_name = line.substr(0, curr).c_str();

      curr = curr + 1;
      next = line.find(",", curr);

      pose1 = (double)atof(line.substr(curr, next - curr).c_str());
      curr = next + 1;
      next = line.find(",", curr);

      pose2 = (double)atof(line.substr(curr, next - curr).c_str());
      curr = next + 1;
      next = line.find(",", curr);

      pose3 = (double)atof(line.substr(curr, next - curr).c_str());

      while (flag == 0)
      {

        curr = next + 1;
        next = line.find_first_of(",]", curr);
        if (next == -1)
        {
          flag = 1;
        }
        else
        {
          wp = line.substr(curr, next - curr).c_str();
          waypoint_connected.push_back(wp);
        }
      }

      waypoint[waypoint_name] = vector<double>{pose1, pose2, pose3};
      connection[waypoint_name] = waypoint_connected;
    }
  }
}

void VisitSolver::parseRegions(string region_file)
{
  // Let's define a structure of the regions file as follow: r*=wp*.
  int curr, next;
  string line;
  ifstream parametersFile(region_file);
  if (parametersFile.is_open())
  {
    while (getline(parametersFile, line))
    {
      curr = line.find("=");
      string region_name = line.substr(0, curr).c_str();

      curr = curr + 1;
      next = line.find(".", curr);

      string wp = line.substr(curr, next - curr).c_str();

      region[region_name] = wp;
    }
  }
}

void VisitSolver::parseLandmark(string landmark_file)
{

  int curr, next;
  string line;
  double pose1, pose2, pose3;
  ifstream parametersFile(landmark_file);
  if (parametersFile.is_open())
  {
    while (getline(parametersFile, line))
    {
      curr = line.find("[");
      string landmark_name = line.substr(0, curr).c_str();

      curr = curr + 1;
      next = line.find(",", curr);

      pose1 = (double)atof(line.substr(curr, next - curr).c_str());
      curr = next + 1;
      next = line.find(",", curr);

      pose2 = (double)atof(line.substr(curr, next - curr).c_str());
      curr = next + 1;
      next = line.find("]", curr);

      pose3 = (double)atof(line.substr(curr, next - curr).c_str());

      landmark[landmark_name] = vector<double>{pose1, pose2, pose3};
    }
  }
}

void VisitSolver::find_path(string region_from, string region_to)
{
  int temp = 0;
  double f, g_s, h_s;
  double i;
  string waypoint_init, waypoint_curr, waypoint_goal, node;
  vector<double> waypoint_curr_data, waypoint_succ_data;
  vector<string> successors;               // In this vector there are all the successor waypoints of the current waypoint (by name)
  map<string, vector<double>> open, close; // both with the structure [wp*, {g(wp), h(wp), f(wp) = g(wp) + h(wp)}]
  map<string, string> parent;              // parent of a given node

  // translate the regions in waypoints
  waypoint_init = region.at(region_from); // initial waypoint
  waypoint_goal = region.at(region_to);   // goal waypoint

  // Now we use the A* algorithm to find a solution
  open.clear();
  f = distance_euc(waypoint_init, waypoint_goal);
  open[waypoint_init] = {0, f, f}; // put node_start (waypoint_init) in open with its heuristic (f)

  // while open is not empty
  while (!open.empty())
  {
    i = 1000.0;
    for (auto i_waypoint = open.begin(); i_waypoint != open.end(); ++i_waypoint)
    { // from the open list take the node node_curr (wp_curr) with the lowest heuristic (f)
      if ((i_waypoint->second[2]) <= i)
      { // even the < would be sufficient and if two valeus are equals would keep the oldest, with the <= i will keep the newest
        i = i_waypoint->second[2];
        waypoint_curr = i_waypoint->first;
      }
    }

    // if node_current (waypoint_curr) = node_goal (waypoint_goal), we found the path
    if (waypoint_curr == waypoint_goal)
    {
      cout << "path found" << endl;

      // recovering the path and its cost
      path.clear();                             // clear all the vector from previous iterations
      cost = 0;                                 // init the cost to 0 from previous iterations
      path.insert(path.begin(), waypoint_curr); // first insert the goal
      node = waypoint_curr;                     // initialize the node to the goal one
      while (node != waypoint_init)
      {
        path.insert(path.begin(), parent[node]);        // insert the parent of the actual node at the beginning of the path vector
        cost = cost + distance_euc(node, parent[node]); // add the step cost
        node = parent[node];                            // set the new node as the parent of the actual one for the next iteration
      }
      ShowResults("./visits_domain/path.txt"); // write the result both on the terminal and on the 'path.txt' file
      return;
    }

    // otherwise generate all the successors of the current node
    else
    {
      successors = connection[waypoint_curr]; // generate the successors nodes, that are all the one connected without the one where we come from

      // for each successor
      for (auto i_succ : successors)
      {
        waypoint_curr_data = open[waypoint_curr];
        g_s = waypoint_curr_data[0] + distance_euc(waypoint_curr, i_succ); // set the successor_curr_cost to g(waypoint_curr) + cost(waypoint_curr - waypoint_successor)

        // if the successor is already in the open list
        try
        {
          waypoint_succ_data = open.at(i_succ);
          if (waypoint_succ_data[0] <= g_s)
          {
            goto done;
          }
        }
        catch (const std::out_of_range &oor)
        {
          // if the successor is not already in the open list
          try
          {
            // if the successor node is already in the close list and it's not in the open list
            waypoint_succ_data = close.at(i_succ);
            if (waypoint_succ_data[0] <= g_s)
            {
              goto done;
            }
            else
            {
              close.erase(i_succ);               // remove the waypoint from the close list
              open[i_succ] = waypoint_succ_data; // put it in the open list
            }
          }
          catch (const std::out_of_range &oor)
          {
            // if the successor is not already neither in the open list nor in the close list
            h_s = distance_euc(i_succ, waypoint_goal); // heuristic of the successor to the goal
            waypoint_succ_data = {0, h_s, h_s};
            open[i_succ] = waypoint_succ_data; // add the successor in the open list
          }
        }
        waypoint_succ_data = open.at(i_succ);
        waypoint_succ_data[0] = g_s;                                           // set the g(node successor) = g_s
        waypoint_succ_data[2] = waypoint_succ_data[0] + waypoint_succ_data[1]; // evaluate f
        open[i_succ] = waypoint_succ_data;                                     // add the node successor to the open list
        parent.erase(i_succ);                                                  // clear the parents of the successor
        parent[i_succ] = waypoint_curr;                                        // set the parent of the node successor to node current
      done:
        continue;
      }
      close[waypoint_curr] = waypoint_curr_data; // add current node to the close list
      open.erase(waypoint_curr);                 // remove the current node from the open list
    }
  }
  if (waypoint_curr != waypoint_goal)
  {
    cout << "error, open is empty, not avaiable path" << endl;
  }
}

double VisitSolver::distance_euc(string waypoint_from, string waypoint_to)
{
  vector<double> from;
  vector<double> to;
  double distance;
  from = waypoint.at(waypoint_from);
  to = waypoint.at(waypoint_to);
  distance = sqrt(pow((from[0] - to[0]), 2) + pow((from[1] - to[1]), 2));
  return distance;
}

void VisitSolver::ShowResults(string path_file)
{
  ofstream output;
  output.open(path_file, std::ios_base::app);
  for (auto i : path)
  {
    output << i << endl; // This line write on the file
    cout << i << endl;   // This line write on the terminal
  }
}