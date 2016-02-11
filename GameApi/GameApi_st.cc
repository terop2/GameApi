#include "GameApi_h.hh"

EXPORT GameApi::TR GameApi::StateChangeApi::init(int paths)
{
  return add_timerange(e, paths);
}
class PFloatRenderer : public Renderer<float>
{
public:
  PFloatRenderer(std::function<GameApi::P (float)> f, GameApi::Env &e) : f(f),e(e) { }
  FaceCollection *Index(float t) const
  {
    GameApi::P p = f(t);
    return find_facecoll(e, p);
  }
private:
  std::function<GameApi::P (float)> f;
  GameApi::Env &e;
};

EXPORT GameApi::VV GameApi::StateChangeApi::prepareloop(float *array, int arraysize,
						 std::function<P (float val)> f, float step_duration)
{
  if (arraysize<2) { std::cout << "Error: arraysize<2" << std::endl; GameApi::VV v; v.id = 0; return v; }
  TR t = init(1);
  for(int i=0;i<arraysize-1;i++)
    {
      float val1 = array[i];
      float val2 = array[i+1];
      t = linear_1(t, 0, f, val1, val2, step_duration);
    }
  VV v = prepare_1(t);
  return v;
}

EXPORT GameApi::TR GameApi::StateChangeApi::linear(TR tr, int path_num, std::function<P (float val)> f, float start_v, float end_v, float duration)
{
  return linear_1(tr,path_num, f, start_v, end_v, duration);
}
GameApi::TR GameApi::StateChangeApi::linear_1(TR tr, int path_num, std::function<P (float val)> f, float start_v, float end_v, float duration)

{
  TROArray *arr = find_timerange(e, tr);
  TROArray *arr2 = arr->copy();
  arr2->push_back(path_num, 
		 new DefaultTimeRange<float>(start_v, end_v, duration),
		 new PFloatRenderer(f,e));
  return add_timerange(e, arr2);
}

EXPORT GameApi::VV GameApi::StateChangeApi::prepare(TR sc)
{
  return prepare_1(sc);
}
GameApi::VV GameApi::StateChangeApi::prepare_1(TR sc)
{
  return add_timerange_vertexarray(e, sc);
}
EXPORT void GameApi::StateChangeApi::render(VV v, float time, SH shader)
{
  ShaderPriv2 *p = (ShaderPriv2*)api.priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  VArray *arr = find_timerange_vertexarray(e, v);
  arr->render(time, prog);
}
EXPORT void GameApi::StateChangeApi::render(VV v, float time, SH shader, float (*fptr)(int path, std::string name))
{
  ShaderPriv2 *p = (ShaderPriv2*)api.priv;
  ShaderSeq *seq = p->seq;
  Program *prog = seq->prog(p->ids[shader.id]);
  VArray *arr = find_timerange_vertexarray(e, v);
  arr->render(time, prog, fptr);
}

EXPORT GameApi::StateChangeApi::StateChangeApi(GameApi::Env &e, GameApi::ShaderApi &api)
: e(e),api(api) { }
