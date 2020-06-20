
class Renderer {

 public:
  void Render() {
    BeforeRender();
    DoRender();
    AfterRender();
  }

 private:
  void BeforeRender() {
  }

  void AfterRender() {
  }

  void DoRender() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw(context);
  }
};
