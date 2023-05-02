import subprocess

CMD = ["/opt/webkit-content-filter-validator/webkit-content-filter-validator"]
CWD = "/tmp"  # binary creates temp files, only /tmp is writeable to its UID
SUCCESS = 0


def handler(event, context):
    try:
        ps = subprocess.run(CMD, text=True, cwd=CWD, check=False, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, input=event["body"], timeout=60)
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
