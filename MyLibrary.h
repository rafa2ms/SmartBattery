class MLS{
public:
  float A;
  float B;

  MLS (float a, float b): A(a), B(b) {}

  void add_value(float a, float b) {
      A = a;
      B = b;
  }
} ;

struct wifidata{
  String SSID;
  String PSW;
};

struct status{
	bool ap	= 0;
	bool wifi = 0;
	bool broker= 0;
};



