from fastapi import FastAPI,  HTTPException # File, UploadFile,
from pathlib import Path
import packs
from main import packmapping_file
import json

app = FastAPI()

@app.get("/find/pack/{pack_name}")
async def find_pack(pack_name: str):
    pack = packs.get_filedata("packmapping")

    try:
        return pack[pack_name]
    except:
        raise HTTPException(status_code=500, detail="Something happened")



if __name__ == "__main__":
    import uvicorn

    server_port = 13154
    print(f"Starting ModLab API http://127.0.0.1:{server_port}/")
    uvicorn.run(app, host="127.0.0.1", port=server_port)
