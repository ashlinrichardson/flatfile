/* 20190212 dd_apply.cpp apply data dictionary to translate fixedwidth to csv */
#include"misc.h"
#include<ctime>
#include<string>
#include<fstream>
#include<iostream>
#include<algorithm>
using namespace std;

int main(int argc, char ** argv){
  if(argc < 3) err("usage: dd_apply.cpp [data dictionary.csv] [data input.dat]");
  str ddf(argv[1]);
  str dtf(argv[2]);
  str ofn(dtf + str("_dd_apply.csv"));

  cout << "data dictionary file: " << ddf << endl;
  cout << "data input file: " << dtf << endl;
  cout << "output file: " << ofn << endl;
  ifstream infile(ddf);
  if(!infile.is_open()) err(str("failed to open file") + ddf);

  str line("");
  vector<int> start;
  vector<int> stop;
  vector<int> length;
  vector<str> label;
  vector<str>::iterator it;
  register long unsigned int ci = 0;

  /* process file line by line */
  while(getline(infile, line)){
    ci +=1;
    vector<str> w(split(line, ','));
    for(it = w.begin(); it != w.end(); it++){
      strip(*it);
      std::transform(it->begin(), it->end(), it->begin(), ::tolower); // to lower case
    }
    if(ci==1){
      if(w[0] != str("start")) err(str("expected field 0 ") + w[0]);
      if(w[1] != str("stop")) err(str("expected field 1 ") + w[1]);
      if(w[2] != str("length")) err(str("expected field 2 ") + w[2]);
      if(w[3] != str("label") && w[3] != str("name abbrev")) err(str("expected field 3 ") + w[3]);
    }
    else{
      start.push_back(atoi(w[0].c_str()));
      stop.push_back(atoi(w[1].c_str()));
      length.push_back(atoi(w[2].c_str()));
      label.push_back(w[3]);
      if(atoi(w[1].c_str()) + 1 - atoi(w[0].c_str()) != atoi(w[2].c_str())){
        err("length mismatch error");
      }
    }
  }

  cout << start << endl << stop << endl << length << endl << label << endl;

  cout << "loading input file.." << endl;

  mfile dfile(dtf, "rb");

  ofstream outfile(ofn);
  if(!outfile.is_open()) err(str("failed to write-open file:") + ofn);

  /* linefeed not a thing */
  if(label.back() == str("LINEFEED")) label.pop_back();

  outfile << join(",", label) << "\n";

  str d;
  ci = 0;
  unsigned int i;
  unsigned int ul = label.size();
  vector<str> row;
  time_t t0; time(&t0);
  time_t t1;

  cout << "applying data dictionary.." << endl;
  while(dfile.getline(line)){
    if((++ci) % 100000 == 0){
      size_t dfile_pos = dfile.tellg();
      size_t dfile_len = dfile.len();
      time(&t1);
      time_t dt = t1-t0;
      float mbps = (float)dfile_pos / ((float)dt * (float)1000000.);
      float eta = (float)dt * ((float)dfile_len - (float)dfile_pos) / ((float)dfile_pos);
      float p = 100. * (float)dfile_pos / (float) dfile_len;
      cout << "ddap %" << p << " eta: " << eta << "s MB/s " << mbps << endl;
    }
    row.clear();
    for(i = 0; i < ul; i++){
      d = line.substr(start[i] - 1, length[i]);
      strip(d);
      replace(d.begin(), d.end(), ',', ';');
      row.push_back(d);
    }
    outfile << join(",", row) + str("\n");
  }
  return 0;
}
