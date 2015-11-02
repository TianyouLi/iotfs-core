struct Property {
  1:string name;
	2:string value;
}

service OnvifAdapter {
  list<Property> getProperties();
	void moveX(1:i32 x);
	void moveY(1:i32 y);
  void stopXY();
}