
from ultralytics import YOLO

def export_to_onnx():
    model = YOLO("yolo26n.pt")

    model.export(
        format="onnx",
        imgsz=640,
        simplify=True,
        dynamic=False
    )

    print("Export to ONNX done.")

if __name__ == "__main__":
    export_to_onnx()
