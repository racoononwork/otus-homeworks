@0xdbb9ad1f14bf0b36;

struct Shape {
  id @0 :UInt32;
  type @1 :Type;
  x @2 :Float32;
  y @3 :Float32;
  width @4 :Float32;
  height @5 :Float32;

  enum Type {
    rect @0;
    line @1;
  }
}

struct Document {
  shapes @0 :List(Shape);
}