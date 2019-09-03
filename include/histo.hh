#ifndef HISTO_HH
#define HISTO_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;
namespace FastPartons{
  
  class Histo {
  public:
    
    //    Histo();
    //    ~Histo();
    
    //constructor
    Histo(double xmin, double xmax, double binWidth);

    void fill(double entry);
    void fill(double entry, double weight);
    void scale(double scalefac);
    void write(const char *outfile);
    double underflow();
    double overflow();
    void addOverflow();
    void addUnderflow();
    double lowerBound(int bin);
    double upperBound(int bin);
    double integral();
    void sumw2();
    //virtual ~Histo();
    
  private:
    double min;
    double max;
    double binWidth;
    int binCount;
    int countsOver, countsUnder;
    double weightsOver, weightsUnder;
    vector<double> weights;
    vector<int> counts;
    bool useErrors;
  };

  //  class Histo2d: public Histo {
  //  friend class Histo;
  class Histo2d {
  public:
    //constructor
    Histo2d(double xmin, double xmax, double binWidthx, double ymin, double ymax, double binWidthy);
    void fill(double xentry, double yentry);
    void fill(double xentry, double yentry, double weight);
    void scale(double scalefac);
    void write(const char *outfile);
    int bins();                    
    int count(int bin);         
    double lowerBoundx(int bin);
    double upperBoundx(int bin);
    double lowerBoundy(int bin);
    double upperBoundy(int bin);
    double integral();
    
  private:
    double minx;
    double maxx;
    double miny;
    double maxy;
    double binWidthx;
    double binWidthy;
    int binCountx;
    int binCounty;
    vector< vector<double> > weights2d;
  };
}

#endif
