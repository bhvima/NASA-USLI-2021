//http://www.schwietering.com/jayduino/filtuino/index.php?characteristic=bu&passmode=hp&order=4&usesr=usesr&sr=100&frequencyLow=0.36&noteLow=&noteHigh=&pw=pw&calctype=double&run=Send

//Low pass butterworth filter order=4 alpha1=0.02 
class  FilterBuLp4
{
	public:
		FilterBuLp4()
		{
			for(int i=0; i <= 4; i++)
				v[i]=0.0;
		}
	private:
		double v[5];
	public:
		double step(double x) //class II 
		{
			v[0] = v[1];
			v[1] = v[2];
			v[2] = v[3];
			v[3] = v[4];
			v[4] = (1.329372889873753349e-5 * x)
				 + (-0.71991032729187132144 * v[0])
				 + (3.11596692520174567420 * v[1])
				 + (-5.06799838673418978630 * v[2])
				 + (3.67172908916193563300 * v[3]);
      
      double res = (v[0] + v[4])
				+4 * (v[1] + v[3])
				+6 * v[2];
      
      if (abs(res) < 0.1)
			  return 0;
      return res; 
				 
		}
};