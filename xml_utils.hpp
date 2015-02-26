namespace PrositCore {
static int solve_core(vector<GenericTaskDescriptor*> & v, 
                      vector<double> & probability, 
                      vector<double> & quality, 
                      vector<long long> & time);
static int solve_display_results(vector<GenericTaskDescriptor*> & v, 
                                 const vector<double> & probability, 
                                 const vector<double> & quality, 
                                 const vector<long long> & time, 
                                 bool show_time);
static int solve_execute();
static int opt_execute(XMLParser::Parser * p);
}