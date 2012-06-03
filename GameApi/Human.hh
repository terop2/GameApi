

class Human
{
public:
  Human(Point pos, Vector up, Vector left, Vector forward, float head, float neck, float body, float leg_1, float leg_2, float leg_3, float left_leg_x, float right_leg_x) : pos(pos), up(up), left(left), forward(forward), head(head), neck(neck), body(body), leg_1(leg_1), leg_2(leg_2), leg_3(leg_3) { }

  float head_radius() const { return left.Dist() / 3.0; }
  Point head_top() const { return pos + up; }
  Point head_bottom() const { return head_top() - up*head; }
  Point neck_top() const { return head_bottom(); }
  Point neck_bottom() const { return neck_top() - up*neck; }
  Point body_top() const { return neck_bottom(); }
  Point body_bottom() const { return body_top() - up*body; }
  Point left_leg_top_1() const { return body_bottom() + left*left_leg_x; }
  Point left_leg_bottom_1() const { return left_leg_top_1() - up*leg_1; }
  Point right_leg_top_1() const { return body_bottom() - left*right_leg_x; }
  Point right_leg_bottom_1() const { return right_leg_top_1() - up*leg_1; }
private:
  Point pos;
  Vector up, left, forward;
  float head, neck, body, leg_1, leg_2, leg_3;
};



class HumanElem : public FaceCollectionArray
{
public:
  HumanElem(int numfaces_head,
	    int numfaces_neck,
	    int numfaces_body,
	    int numfaces_leg, 
	    Human &h) 
  : head(numfaces_head, h.head_top(), h.head_bottom(), h.head_radius(), h.head_radius()),
    neck(numfaces_neck, h.neck_top(), h.neck_bottom(), h.neck_radius(), h.neck_radius()),
    body(numfaces_body, h.body_top(), h.body_bottom(), h.body_radius(), h.body_radius()),
    left_leg_1(numfaces_leg, h.left_leg_top_1(), h.left_leg_bottom_1(), h.leg_radius(), h.leg_radius()),
    right_leg_1(numfaces_leg, h.right_leg_top_1(), h.right_leg_bottom_1(), h.leg_radius(), h.leg_radius()),
    left_leg_2(numfaces_leg, h.left_leg_top_2(), h.left_leg_bottom_2(), h.leg_radius(), h.leg_radius()),
    right_leg_2(numfaces_leg, h.right_leg_top_2(), h.right_leg_bottom_2(), h.leg_radius(), h.leg_radius()) { }
    
private:
  ConeElem head;
  ConeElem neck;
  ConeElem body;
  ConeElem left_leg_1;
  ConeElem right_leg_1;
  ConeElem left_leg_2;
  ConeElem right_leg_2;
};
