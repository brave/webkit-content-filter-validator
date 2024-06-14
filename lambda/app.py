import subprocess
import base64
import zstandard as zstd

CMD = ["/opt/webkit-content-filter-validator/webkit-content-filter-validator"]
CWD = "/tmp"  # binary creates temp files, only /tmp is writeable to its UID
SUCCESS = 0

dctx = zstd.ZstdDecompressor()

def handler(event, context):
    try:
        if event.get('headers', {}).get('content-encoding') == 'application/zstd':
            if event.get('isBase64Encoded', False):
                body = dctx.decompress(base64.b64decode(event["body"])).decode('utf-8')
        else:
            body = event["body"]
        ps = subprocess.run(CMD, text=True, cwd=CWD, check=False, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, input=body, timeout=60)
        if ps.returncode == SUCCESS:
            return {
                "statusCode": 200,
                'isBase64Encoded': False,
                'headers': {
                    'content-type': 'text/plain'
                },
                "body": ps.stdout
            }
        else:
            return {
                "statusCode": 400,
                'isBase64Encoded': False,
                'headers': {
                    'content-type': 'text/plain'
                },
                "body": ps.stdout
            }
    except Exception as e:
        return {
            "statusCode": 400,
            'isBase64Encoded': False,
            'headers': {
                'content-type': 'text/plain'
            },
            "body": f"An exception occurred: {type(e).__name__} - {e}"
        }
