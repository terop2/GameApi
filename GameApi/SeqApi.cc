
P TimeObj::frame(float time) const
{
  set_points(time, *api.Points());
  generate_objects(time);
  return or_array(&objects_in_current_frame[0], objects_in_current_frame.size());
}

void TimeObj::decide_visibility(float time) const
{
  if (time <start_time || time>=start_time+duration)
    return false;
  else
    return true;
}
void TimeObj::generate_objects(float time)
{
  objects_in_current_frame = std::vector<P>();
  
  int sz = objects.size();
  for(int i=0;i<sz;i++)
    {
      int idx = objects[i];
      SeqObj *obj = api.Objects()->At(idx); 
      objects_in_current_frame.push_back(obj->draw(api.Points()));
    }
}
		  
void SeqApi::push_obj(int type)
{
  switch(type)
    {
    case EQuad: objects.push_back(new QuadSeqObj); break;
    case ECube: objects.push_back(new CubeSeqObj); break;
    case ESphere: objects.push_back(new SphereSeqObj); break;
    case ECone: objects.push_back(new ConeSeqObj); break;
    case ERing: objects.push_back(new RingSeqObj); break;
    };
  return objects.size()-1;
}

void SeqApi::link_obj_point(int object, int point, int obj_point)
{
  objects[object]->set_point(obj_point, point);
}

int SeqApi::push_time_obj(int time_type float start_time, float end_time)
{
  switch(time_type)
    {
    case ELine: objects.push_back(new LineTimeObj(*this, start_time, start_time-end_time)); break;
    case ECurve: objects.push_back(new CurveTimeObj(*this, start_time, start_time-end_time)); break;
	     			 
    };
  return objects.size()-1;
}
int SeqApi::link_time_point(int time_obj, float time, int point, Point pos)
{
  TimeObj &obj = timeline[time_obj];
  obj.set_point(point, time, pos);
}

void SeqApi::link_timeobj_and_obj(int time_obj, int object)
{
  TimeObj &obj = timeline[time_obj];
  obj.push_object(object);
}

P SeqApi::frame(int time) const
{
  std::vector<P> vec;
  int sz = timeline.size();
  for(int i=0;i<sz;i++)
    {
      TimeObj &obj = timeline[i];
      bool b = obj.decide_visibility(time);
      if (b)
	{
	  P p = obj.frame(time);
	  vec.push_back(p);
	}
    }
  return or_array(&vec[0], vec.size());
}

void SeqObj::init()
{
  points.reserve(NumPoints());
}
