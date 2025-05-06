#include "SWparser.h"
#include "SW_http.h"
#include <typeinfo>
//_______________________________________________________________________________________________
extern bool OTA_enabled;
//_______________________________________________________________________________________________
const char* VERSION=_VERSION_; 
const char* LOGO="iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAIAAAD/gAIDAAABJmlDQ1BBZG9iZSBSR0IgKDE5OTgpAAAoz2NgYDJwdHFyZRJgYMjNKykKcndSiIiMUmA/z8DGwMwABonJxQWOAQE+IHZefl4qAwb4do2BEURf1gWZxUAa4EouKCoB0n+A2CgltTiZgYHRAMjOLi8pAIozzgGyRZKywewNIHZRSJAzkH0EyOZLh7CvgNhJEPYTELsI6Akg+wtIfTqYzcQBNgfClgGxS1IrQPYyOOcXVBZlpmeUKBhaWloqOKbkJ6UqBFcWl6TmFit45iXnFxXkFyWWpKYA1ULcBwaCEIWgENMAarTQZKAyAMUDhPU5EBy+jGJnEGIIkFxaVAZlMjIZE+YjzJgjwcDgv5SBgeUPQsykl4FhgQ4DA/9UhJiaIQODgD4Dw745AMDGT/0ZOjZcAAAACXBIWXMAAAsTAAALEwEAmpwYAAAGmWlUWHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78iIGlkPSJXNU0wTXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRhLyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgNS42LWMxNDIgNzkuMTYwOTI0LCAyMDE3LzA3LzEzLTAxOjA2OjM5ICAgICAgICAiPiA8cmRmOlJERiB4bWxuczpyZGY9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPiA8cmRmOkRlc2NyaXB0aW9uIHJkZjphYm91dD0iIiB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8iIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvbW0vIiB4bWxuczpzdEV2dD0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL3NUeXBlL1Jlc291cmNlRXZlbnQjIiB4bWxuczpwaG90b3Nob3A9Imh0dHA6Ly9ucy5hZG9iZS5jb20vcGhvdG9zaG9wLzEuMC8iIHhtbG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIgeG1wOkNyZWF0b3JUb29sPSJBZG9iZSBQaG90b3Nob3AgQ0MgMjAxOCAoV2luZG93cykiIHhtcDpDcmVhdGVEYXRlPSIyMDIxLTExLTE0VDIwOjU0OjUzKzAzOjAwIiB4bXA6TWV0YWRhdGFEYXRlPSIyMDIxLTExLTE0VDIwOjU0OjUzKzAzOjAwIiB4bXA6TW9kaWZ5RGF0ZT0iMjAyMS0xMS0xNFQyMDo1NDo1MyswMzowMCIgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDo2NDFlYjc4Mi1jNGFmLTQ0NDUtODE5Yy0zMjY0OTRlYTM1NzIiIHhtcE1NOkRvY3VtZW50SUQ9ImFkb2JlOmRvY2lkOnBob3Rvc2hvcDoxOWM3MWU4ZS1lOGVmLTkwNDAtOTZlYy1kYjgyMGU5MDVmMTYiIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDo5ZDkyZGY0NC00MjU2LWIxNGYtYjMxYi1iMTZlMDc1N2RhYWYiIHBob3Rvc2hvcDpDb2xvck1vZGU9IjMiIGRjOmZvcm1hdD0iaW1hZ2UvcG5nIj4gPHhtcE1NOkhpc3Rvcnk+IDxyZGY6U2VxPiA8cmRmOmxpIHN0RXZ0OmFjdGlvbj0iY3JlYXRlZCIgc3RFdnQ6aW5zdGFuY2VJRD0ieG1wLmlpZDo5ZDkyZGY0NC00MjU2LWIxNGYtYjMxYi1iMTZlMDc1N2RhYWYiIHN0RXZ0OndoZW49IjIwMjEtMTEtMTRUMjA6NTQ6NTMrMDM6MDAiIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFkb2JlIFBob3Rvc2hvcCBDQyAyMDE4IChXaW5kb3dzKSIvPiA8cmRmOmxpIHN0RXZ0OmFjdGlvbj0ic2F2ZWQiIHN0RXZ0Omluc3RhbmNlSUQ9InhtcC5paWQ6NjQxZWI3ODItYzRhZi00NDQ1LTgxOWMtMzI2NDk0ZWEzNTcyIiBzdEV2dDp3aGVuPSIyMDIxLTExLTE0VDIwOjU0OjUzKzAzOjAwIiBzdEV2dDpzb2Z0d2FyZUFnZW50PSJBZG9iZSBQaG90b3Nob3AgQ0MgMjAxOCAoV2luZG93cykiIHN0RXZ0OmNoYW5nZWQ9Ii8iLz4gPC9yZGY6U2VxPiA8L3htcE1NOkhpc3Rvcnk+IDxwaG90b3Nob3A6VGV4dExheWVycz4gPHJkZjpCYWc+IDxyZGY6bGkgcGhvdG9zaG9wOkxheWVyTmFtZT0ic2VjdXJlIiBwaG90b3Nob3A6TGF5ZXJUZXh0PSJzZWN1cmUiLz4gPHJkZjpsaSBwaG90b3Nob3A6TGF5ZXJOYW1lPSJhdXRoIiBwaG90b3Nob3A6TGF5ZXJUZXh0PSJhdXRoIi8+IDwvcmRmOkJhZz4gPC9waG90b3Nob3A6VGV4dExheWVycz4gPC9yZGY6RGVzY3JpcHRpb24+IDwvcmRmOlJERj4gPC94OnhtcG1ldGE+IDw/eHBhY2tldCBlbmQ9InIiPz4iS0nyAAAacklEQVR42u1dB3hUVdrOv7qmTO+3zKQnkx5QWqQkoYr/8rsKuLr7K6xlsdClC4giVVBRaYoFpSNIUxQUeFAIKL/rGoqiwoKQIiAg6VPyv+ecOzd3ZpIwCAq6c57vudzces57vvJ+55y5RNSHS8glIgxBGKwwWGGwwmCFwQqDdUWeUl5ePmXKlAf9y6hRo44cOYKzL7744oNBBQdxqri4GJcFnMKjKioqcJb9uXXrVvlF7Ah77IONlcWLF+MULmj0LLvxKoOFKkY0VgAEa1XwKRxs6hTK2rVrSeVoYe2XqksLIJb3gwsQwQWNnmI3XitgQV8W0+J0OgMQKSgoWKwoTF/YqT59+rCDuF0JUChgQQ3ZvegY+awMlnyWFVjAtQIWg0a2lwCwlGcDLguGI3SwZGWRAVKCdZmq9CuBFYxIiGCxpjIV+D2DBdNT6jz8juynA8xQbkMwWH6VuxSwioqKmjfDawus4IK6NurF5apfPlgBBe4PPdSog7+GqEMAAwgOedA7+azMBn4JsJQmiT/ll167pPRnO/hLBQvGzuxO1u5r3WdBU2SeGY6GIfksuPBiX4H+B4CFI8WKIrNwxl3l48FgwfGxU0oXHiJYst6xwgLONergm6HpzTN4JVhNEfFQwLoWGXxTlWNZSzNgyZRdWRAKmN7JpDzgLFOQ3ypY4VGHcAmDFQYrDNZvASyvx+txe7xuj3zg6JHvNm3aOG/evGlTp4weNXL4sCFDBg8cPGjgsKGDhw0bMmjgwJtuapnpdGalOXMy0rPTU5nkMElLI5KelpqY8ED/fq++vHD86NGZqeR4dqoTO09MGL9wwfy777rT4bD36X37woXzX3zh+Vt6dJs1a+b8eXMXzJu7cN7clxfMnzN71k25OePHjlk4b978uS/h+Mvz572yYMGt3bunp6Zkp6fh7fKrZcmikp6a3CI3e9DARx9/fNzECRMmThw/YcL4qVOnPDt71quLXn5/87tff3WwpqbK11y3x+PyQrxu7NfXe2QJAstb73G53XUu7FZeqHht0as9e3SPixWNBp1Wo9Jp1VpNjE4bY9CrjQaN0aA16DV6nUrkLLECB4kjWxuTOCY8D4nlcUow63UGtdpmMMQLAg7G8wKO63UaPNZqMdlFzmY1q2Ki8CLs0Hep9Bq1UauBmHRaO2fDVq9WSUfoVrTZ4kQhVuAdvPRqhxgsnChwBr3WqNfrdVo8Vq2Kxos0arRFbTLq8Or2N7d9fNzo4uIvGAwedx3wughYXq8XeKHs2Latw83t8ViL0WgHBHYhzi46UC07j9djG+fg7YJVFKx20aYEqFGwCF6kPbz8Z4IgMsgEzuZwiLEOEVu7yIsQgRN4G7b4Ey9lWEDYDruLPMFOnuDgeIBot1ntNpuDt9p5VMniL1aet/CcVeRtdlzMo9poAracb8cKsaAfNNFo1NAhg0pOnpAUR4GUH1geWoAW9l+a8wJvtRm0OjtpJKsr59vSDhQ5IgKpn0P0QycQLKgbz+6lgn2iZXy8KGALsaMNtDOwdUBEHluRHiTHcRnFKI7toD4cB4F6mjQas1bLWyw2i4kJZzXZiJiVYpXEYiVXWnirlbfZOKuViUmvg6oaNCrBaoZu8lazRhXdskXOli3vU9Xx1ywvBRAK5SbFBbyenDQRioq+hR6JgtxOCoFojVXA8asIx2BiAlWyGQ0Wvb57YeH0p59+b8P6XZ/sLCratXv3J7LgT6XslkrR7qIi8g8t7B/8uX37trcWv/7IQwNaZGfB5K0mI9QZPgeauOStxQF4NYAFv45zT016EkhxNgvusVOLuHbAgpaZ9Nr2eW3XvbO2trbmyka6stKS556dleZMQfPRaqikyah/e/VKZnVKMyRY4Z+3Fi/WaTTQKYbUNQUWTFKv1Txw/99Pnz7FGoA+99a7AyylMWmyeFmhsY8dOXLk29633wavj7bDjTns4qef7pH1K4JoFL30q0MHk+JiYdIyUtcIWMRh2UV0+MOPDHC561jEJmBRqb+4XLT4XVxXV/vQgAeZfiFSI0qeP3eWvM7jiqD/uF11tX3vuN2gVpGQZxdkAV5XGSzehlBg1Glvu60XTM8bFKGuNFhEwLn+evdfQCxgZGAY06dOofTTRTQLe+vWvg33ZudI9FGCBWFgOWgnOySwpJgYJ8rxkZNMRt4RiTh4m/J6f31pSgI1i7eYM9Oc3357GPV0wWR+SbBgbh6qvHBhrW5qaTLoeZs1JTHh2JHvABc0y1tXVwPGDJUTKbtRqhXs1mwyMEGMMJv0dsoDEGVZROcsZrNBbzEarAhSdEcZvMCAOLMJ8ctmMrILOJMxwUeUJLrEgZ3yiiMc4x/oHjA4UbChhxcterkxFWBUiEkokHgZM/L6l6YgXrdurdGoJ8oVHfXUpCekdOfjnTssZiOCpUDqxyvBKsjv2KN71+7dulDpmt+pQ6yPIjLu07lTx25dO3ftUgjBTl7b1sCRkmnCFRNjHV0L8nt06dy9cyG2t3Tr2ikvz2GzgQEQ4ck2gZf2GaeP57l4GSw7oNbn53esqqqgFqBQAY+rtqaqrq7aVVcDH+Jy1dKdYKl11cIL1bpdbuLKA4FqBixwKVffvr2hQ9CYtm1aXfjpHAFrxIjh8P/QKUaaZaSgTdu3fcjSJUr8vadOld+YmwO7ABxoR17rVhXnz7Fg4aFR4sT3x5xJSeDDuACqVNChfV1VJcuhvFS9Vy5dYtHpJLAEgTMajRoNHpXgOxJP+CoxYZBeh53T6zVvvPEa5cwuZbd/8c//Q0cWFnTqUljQpSCfSH4nSQqUkt+lsPDmdnmzZ82uV+hU8z6L2SNOvP/BZmJwAge8dmz/KKK6uvLmvLb4A/rGkgySedgJHYX+v/H6qw1JYz2JBD27dwUKIpiyTnvP3XcFBWNPz25dkQMCLGz/emdfv7Szvv7JJyaAMSNPhN7BKrsVFjzy8IAOeW3BM4PBQi6YnZWOHmKsWcES6nft+hjNgDAP0LxEXn89cn4pS0afUTf9Q3lZTXUl8YOuWhwMoCD0dZ7Kygvtb24HpUFwfHrykxFgDA47D5iIGVKGpQRr/ONjffbuZgzj/v790FoCllYz7enJwaF3yMBH9aoYgIUsd9yokZJi+s72v+d/cRxGiu3AhwZUVpLR9LM/nun959sMKlU8yRkbwNLrYgYPeoSpVQBYoObgjbzN4lDmUsw58P4i8AaNetSI4RI787grKn4CdikpSR073Lxz5w7UD3j5a65HtpVxY0cjIOJdd/btHfHeu5uQjjNKpfTuEPTb3Xfd6aNvUpiYMG6sjmIBxXl3w/pgsBYtmI8LUGlg+tLzz7Ggy7wA6tS5IB9GB0NOT0kpo/lqTQ3p3n/u+0y0WmMtVhks+CyTUbt+3RqcZT2vBAtpDRg2OjgYnXh/kcB6bLivqvWzZky/4bo/2MwmrSomNyerrKwUShSsXAys9evWGqgl3tgyN2L+3HlGjS5OtEOZRB6uqsG7w+vDQquJrnp9gzv1i15eiFQTAdUhil8f+io46CAbs5rMDkFEHr5p/QZFeuUtKfk+PS2V5BImY4+uXagjA82sxU7FT+dbt2zBm40kP7dzcXabwJmSkuKPHfu37Gt8UU8CC9ZBwPKpD3ZQK9ITJipmEyIv8LaYjJE3XP/Y8KHy2Evngk5GvRah34Ee1ao3blxH+8PFWJQvvkqO7eDBg4lx8Q5eSE9OjZg8cZKJgCU6GFhCA1iwypTkxH8f/U5hSvUfbv0AXQoc27ZuU1VRGQzWD2XlmekZVrMFDGnfp58pVK9+z57dHGdlmVdeuzZwGV6Px11bhxaUl5zMSkvlzAY6/sPF28mIVucuhXUuV2O0KBAsygR5dD56N69tGybt2rZu16Z1mzatMjLSpk+fym4GT4L1oQloY1ysHZFtzdur2KALc/xeaWjGy7jJmR9/zM3Ktlu5HGd6xLiRo/QxKvYyZTRk+3D8HxOrbmhw8ZdfAEQ4vP739vN5bf+meDy33tJTp9GmJqec/P6E8t7Vq1dqNCrmHAH3pk0b5LsWzptrREbGWYhaiVy8aDPqYpAJNsEhA8EihM5iRuyuqqqEVyZS8RMTeKgLF84z+2D3jh414oY/Xoc6ALKkxHiiDeg0jzsILPJnVXV1+3Z5vMmSm54ZMWbECHhW4hpFQUmymAsD8G8ufl3Z4NOnyrMy06Mib5g6Zap02NvQLayMfGxE5B9vaNOqdaWketK9U6ZMVsGEOSvjJc7U5OVLlxzafwBIJcfHiTYL4/0MLF1M5PhxYy4JrL17dvsz1QaR/RFsrbyspNefboWnRihj4zBuV61X0qQGpBhYdXV1ndp3sBlMuWkErMdMWjXJMwhSTCQzhACsCePHKc0QaSSoaUx01LsbN0kD9nDgLpcSrDdee/36P1x3x59v9+mdh2ngAw/cp6WahSfDzBBAQI5NOr0q8gbQCNQh3s5DAFYCBWv2MzN/BlgskMkBlIU5GSyPh4Sp6qoKMDXmYYAUBcvbOFguApZVb8xxZgCs4SZtDBv8BAmUwWKCFMcXED1ym/9yZ29U9JvDh6XRfa8H8JNRVl/ZW7QnOjJq8MBBbP6DNc/lquvatbPBoGPD4aBXW7Z8sBnBeP36LZs3g6w6kxKIGQrg91yiYDNER0LjQgSLkGSL+ct//bPpwRi3j5fXwVcCHTkNYH+63Q1mGKhZHToCrMxUpz9YIh3VpRJLxWo2dmifV9Ng8KSiQ4cOzsnOrKmuZmCRwP/554e/+prVipjqD6di7Y4Z06YrwTr1Q1lmZjpaiIerY6JffOF5ZWvOnTntTE7kLCYHScLBtjiDKnrB3BdDAovmqqB+4H2rVixftuQtKm8yWfzGaydOHPdlS/XffXsYqVvnwnywfwgCwswZ04g20lHi+oZQKE1GELA6dgJYWalpEaNHDDcysBoTwWpOTko4duyo0vXg6XfcfhtTeBAU7IB/LF+6jFklfZW3c0HhW4vfVAz7kwSFZAgcmYwBBZv1zAzlM0tKTsCFgdWz3BPKgjQoFM1yKBJV2DK4rlmrNWk0JrUaYlCroyL/uGPHNvnG/cX/wltgMUyuv+6/Bg1s4L3BoxQAKx+aZTBlIxqOHjnMqI0mA1VMBL8RKwfiodm4e9fHyoatWLEM3J+xTSRA2PnHAw+OGzOWKRrTteFDh215/wMlWOveWQMP6KCtahSsNGcKBYu0HBm1Ua1+gXLaEMGSB0JIJs/Z2MwIfJkZ9d/9iXzjwQPF8OsssYPAhw4fNkTq+KbBsjWApVOAFTS8hxYuXfKmsmHIyz54/z0GlpcwJU9hfkHf3n2UYK1dvWb/l8U+wyTa98zM6cifYIONglVaehJ8VQKLJ/NdUIrnZj9zSWAxsXMwSRu2bMdo1O9q6GyQzP1x4NMUKQaWj682Pv5VW1sLsDijWQYrKla0+CQALE6rjnnqySfksQfAf+b0D6dpcsseXVZSmpqU3OrGmyovVEhezEvc1k/nfyJRyOcsoO0kr6IN0wWD1aBZdIqM47VRUbNnzpCmMkMACwLywVssnMVqM1sgnNmKXEKv0+5soIoUrFh76GAxzZLBGmrURcaKZp9Y/MyQalb/fvfIYLGpWqpTLqYyuz/ZZTaaHKL94P4DLFetpxNFRMuIC/PQ9npu6dENFuEIESye10VHz5g2NXSwYHFpyUlZaWlpqU6kEJCMtPTUlJTk5MS9e4suEyzZDJsDC2Iy6PM7dnDV1bCxB0ZhEGtJXs2yxYWvGHR6yLp33pHsjtJA5KbUJElTz5w5hQBK5ujpIEFIYMXETJvydIhgwfoEq2Xzxo1lpXgSHODJkydOku33J44f/3cV8iqvVJNLBQsUMp9Gw+zUEHyWYDFnpqaW0hECyQX69IuBNfDRR/DKmJioSZMmKtMFH3emMWj/l2BtZAioCc1i0RBMVdIUjtNGSzMFzfss9kAHGfuy7PtsX5OzXg2J8YEAsJiDv4gZ6k25hDpcDKw4jkwZfLZ3jxIs39gpUmBXty6FSLLgj+66q28wCabzC/XvvScNBDlCBksXEz1z2tRLAmvPnj3BmujD4NcAi4NFmHW6NatWNDplUFpyEp5CsFlhYq1uanmOTLH5g0U1a86c51CtSwILGevM6dOuBFhyt/18sKx6Uw4Fa7hBE9U8WAa1aurkpwIm/tko9c4d26xGQwJsjCNjrV9++YWyfkToZYMGPQreECpYvKRZc557NnSwOLNl7569Vxys6prqTh07WnQGAtaYUY+BlMbZbU2ZIept1mnv79/Pn7l5mBnOfWGOXqWKpwuDDDrN26tXBYBFwqHH1avXf4MuhwJWrAxWdPRri1755cBiIwUXBaumtqZTp05g8GQ8a/q0yUZdDMCSRAzMeKBZVoOhc6eONPlUjnUQsB59eACoNlnfYhf1avWkJyYGg3X27JkWLXLocrUQwBLJnBgxQ7V6jbQu42qCdaGionXr1jbGs1avWm7Sq5HlAKZ4ItY4wU/AuXmrFfS6tPSEEiw2O4tc1GrQM82CPfa543Y6iuZhFIvlooe/OojqcXQCzUFHGZsHi5ImzmTQbftoa1NgFe3+BAkefKXDR7JsFvOnLAopMmHlgBa7+dCB/fEOu91XE71G3RRY7JbvvvkmKQ6ocJnOtIhDh/aLvEW0mmCAjYLFVuOBAX2271N/t1V//NjR1OREEBxGCxE0W2Rlnjt7RjFJQS7bvnWLSacV0TA2+Ur7M5hnNZihQEbT8dIDB4qbBGvXJ+gbYBSrBOvToqBY7NdyBhY8rDwn1BRYJDDRQYgNa94xa/WCxdaloDCipqaqU4c8m1kPzYIZAq84wU/YiLNeq96wYV1ArT/eucNMBu14ebIeeTfS+oDLXl/0ClSJdSarIlKoOc/7Oe/zZ88Q3G1WlgxbTMYbW+ZWXDjfFFhI9wwaNboHfQDhcINBJw3+NQEWw+vg/mI7nSFGH0NUUZFDBg9sBqyH/zHApNPrNdrJk54kM9JTp07WqKPi7FysqFze2ABWnCBA5//ev9/KFctXLF+6YtnSldguX/boIw+ZjXppTR4sUeDR22NGjVi1Epe8tWLZEvwLl9+3zx3wL3aqUySRFshYyn39+61etXL58qXLli3BoxbMeyk5IZ5pCosV99/Xv97HaYPBQgaTlpqcnZmenZGek5mRlZmONGcf0/0gsJQT0YcOHsjNycL1udlZSCoSE+Lo3KjSvTRo4tGjR5MTEkUbhxyzaNduAtbhw1/FOkSgzVaHBk9SMj9iNRmRJxroGmEIdgBNrNCwDIS1EweNOg0uZVcadFqbzSyPU5OxPWo16FvQVDxQp1EBGgt1fMxVw0ZgHUuWLG4ULMpyvbW11bDcstKTbFtWVgKvV1l1AblFM2Ch1Nbgxu9x/Q/lpeVlJbj97I+nybCywm/IZczYsTqNxmww9uxxS01NTQSDcPDggVpNjJ0oFxcIFsFLYBGdaAe19jjBb12sDJZvibU0dEm8icAJdEkAm+iGZsUL0qgTHuVDkJeXjMaLAqwjJSnxOB1xbBQsrzTAHTiz5Pa6XEhdpcHZxsFC/GHrNhRPdCPzDdAslM8//zw2NhYmiz5d+zaZ643w0Ad+883h5MREFrACZ3SZiLwcy/zWeZJ2SsKWMyvGlYhHJ2uQ7XSoms2GiFRVBWmlDRm/tvvgJnPRRLl0qpgBD96vHIkNyvKoeD1eRntpkkxG1N0uXxSWL2sELKRoJMn3SPe63XVulupKlIjcVFFR0a1bV7OJJHK39uhJxtA93gh5ZH7Os8+pI6MdNi6eI2umZAlyYVdY/BYjgwDRzGnfZ3t9HDiU1Wj1Ta+QCVqN5fEXr5fNADHNohPvHmZqZFzXLlhMph3btrOhpwhZm6srq/50S0+TRpdwlcBiuqaKjpw0aYJy0v8ylyF7L1aUE2XsjsfHjWFrSpGiTRxPKkN+csLAkuclD3/9dUZKqlWnT5B+LsIc1q8HljYm+u/97q2urvJ63L8QWH55q0SbG2b5Kisrhg4ZxFarqWKiev3p1soLFWREygWz9UbInoD9XuejLVsT6BozhXf/lcACcb3v3ntcdbVX5RdfCIibNm4oyO/I1t1i26F93vfHjynhjpCnuOEePXSuYevm91Li4yx6HfvRSCz/a4AFJtHvb389dvRIeWnJyRPHS+hwJ5WTJVeklJJSXl5++tSpH0+fOUOlrLTs6JEj2z768JmZM7p36wIqA0EWERMd2aVzwVE6X81WJgSBhThBfpBC9OvA/uI7/qeXXqWymYzkxzd0ibW00+S6Y/nPnwMWvFXL7OzcjIwMZ2q6MyXNmZKamuyTyy1OtnU6szIzc7OyW2Rlt8zOuTEnNyM1DXkfKB7ir1mvQ/oBYmgxG0eNfOzc2R/lNRBy3IhQ5Jw0PHil6Yba6upXFiy8MbeFTh2DnEZeVhunWICrFHZBY2e5EH4WQES0IUVF+mIWFMLbzDar5WeI1UJ+38Rh32wGCgLNigSrVbRY7GaL3WK1Wyyi2RLLEcqNC/QaFVDrdWvPbR9ulVdysWEoeZQ8gqhUPczPLcfShrBaXw+NXbpkyb33/A2ZQWpKEvoZPZ/pTPX9GDKN7QQc8ZdGfjMZumSlOzMz0i5JMtKdsmTSWrGKZdKak6o6nZAcHIG6JSUlJca3a9t65IjhO3ZsZ7SO8jsXm2RQrkAK/+w3/BvpMFhhsMJghcH6eaWgoIB9NEf5OSb5q0gBR9jXfJr5uFFTN16Rr9RdZbCOHDkiN1X5IdswWI2UtWvXBnwsMRSwAj48Jn9sDLr5ewaLfZ+NfQ0X2xDBaqhc0GfsfrdgQRHkT47JXz0Og9V4gZOSXTtz8/IXFC8fLPmDmnIA+W2DBV8jfzhUuX9FwPolvtl6NcFiroq5Z/nbfuyjyZcPlvwRYPmDgr9hsJr6RCEjEGGf5Vca/Uyk/Dn9MFiNkAYGTXCrgj+2r/wI7H8WWHBMwaxdNsyioiIlWQ3+eOl/FliAg8X1gP8GQf5kd6P/QQhKwEdXg//DD/YRZyWmwUfCow7hIZowWGGwwiUMVhisMFjXRPl/1UcozJoDc60AAAAASUVORK5CYII=";
const char* VERSION_INFO="<span>v.:" _VERSION_ "<span>";//.Обновите приложение по ссылке<br><code>https://drive.google.com/file/d/1V6FXXQQkO-4rLTjgSSXlBSrP5S2rVcmM/view?usp=sharing</code><br>для поддержки всех функций</span>"
const char* ICON="AAABAAEAEBAAAAEAIABoBAAAFgAAACgAAAAQAAAAIAAAAAEAIAAAAAAAAAQAABILAAASCwAAAAAAAAAAAACckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTABwcHP8cHBz/HBwc/xwcHP8cHBz/HBwc/xwcHP8cHBz/HBwc/xwcHP+ckFMAnJBTAJyQUwCckFMAnJBTAJyQUwAAAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/nJBTAJyQUwCckFMAnJBTAJyQUwCckFMAAAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/5yQUwCckFMAnJBTAJyQUwCckFMAnJBTAAAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP+ckFMAnJBTAJyQUwCckFMAnJBTAJyQUwAAAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/nJBTAJyQUwCckFMAnJBTAJyQUwCckFMAAAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/5yQUwCckFMAnJBTAJyQUwCckFMAnJBTAAAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP+ckFMAnJBTAJyQUwCckFMAnJBTAJyQUwAAAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/nJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAAAAAP8AAAD/AAAA/5yQUwCckFMAAAAA/wAAAP8AAAD/nJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwAAAAD/AAAA/wAAAP+ckFMAnJBTAAAAAP8AAAD/AAAA/5yQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAAAAA/wAAAP8AAAD/HBwc/xwcHP8AAAD/AAAA/wAAAP+ckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwAAAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP+ckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAAAAAP8AAAD/AAAA/wAAAP+ckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMAnJBTAJyQUwCckFMA//8AAP//AADgBwAA4AcAAOAHAADgBwAA4AcAAOAHAADgBwAA4AcAAPGPAADxjwAA8A8AAPgfAAD8PwAA//8AAA==";
const char* CERT="-----BEGIN CERTIFICATE-----\n"
     "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n"
     "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
     "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n"
     "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n"
     "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
     "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n"
     "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n"
     "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n"
     "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n"
     "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n"
     "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"
     "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n"
     "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n"
     "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n"
     "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n"
     "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n"
     "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n"
     "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n"
     "-----END CERTIFICATE-----\n";

const char* CREATE_CLIENT_FAILED  ="error: create new client FAILED";
const char* CONNECTION_FAILED     ="error: connection FAILED";
const char* RESPONSE_TIMEOUT      ="error : response timeout";
const char* CONNECTION_TIMEOUT    ="error: connection TIMEOUT";
const char* TRANSFER_ERROR        ="error: transfer error";
const char* NO_AP_CONNECTION      ="error : no connection to AP";
const char* AP_CONNECTION_TIMEOUT ="error : connection to access point timeout";
const char* ACCESS_DENIED         ="error : access denied";
const char* OTA_IN_PROCESS        ="<h1>Update in process</h1>";
//_______________________________________________________________________________________________
//RETURN:2,если команда установки; 1,если получения,иначе 0
//длинная форма:get_command, короткая: command  (получение) или старая форма cmd=get&value
//длинная форма:set_command, короткая: scommand (установка) или старая форма cmd=set&value
int SWparser::scom(const char* command){
  if(command=="")return 0;
  String comm=command;
  int p=com.indexOf(comm);//com-длинная принятая строка команды get_comm.. set_comm.. get_ecomm.. set_ecomm..
  int r=0;

  if(p!=-1 && p+comm.length()==com.length()){
    if(p==0 || (com.indexOf("get_")==0 && com.length()==comm.length()+4))r=1;
    else if(  ((p==4 || com.indexOf("set_")==0)&& com.length()==comm.length()+4) || ((p==1 || com.indexOf("s")==0) && com.length()==comm.length()+1)  )r=2;
  }
  else r=0;//команда не найдена
  
  if((data.Eflag & FC_DBG)!=0 && r)Serial.printf("\nscom: command: %s %s ",(r==1?"get":"set"),comm.c_str());
  
  return r;
}
//_______________________________________________________________________________________________
//char* stc(String str){ return (char*)str.c_str(); }  //to char*
char*  its(int v)
{ 
  static char a[16];memset(a,0,16);//обнуление заполняемого
  itoa( v,a,10 ); 
  return a;
}

int   intv1(){ return atoi( (char*)server->arg(1).c_str() ); }//_server->arg(1) to int
int   intv2(){ return atoi( (char*)server->arg(2).c_str() ); }//_server->arg(2) to int
int   intv(String v){ return atoi( (char*)v.c_str() ); }//v to int

char* chcopyt(char* dest,String src,int sizedest,bool terminator,char termvalue)//копирование с возможной терминацией
{
  int sz=src.length()>sizedest?sizedest:src.length();
  char* ch=(char*)src.c_str();
  memcpy(dest,ch,sz);
  if(terminator && (src.length()<sizedest)) {*(dest+src.length())=termvalue;}
  return dest;
}

bool empty(String s){
  return s=="" || s.length()==0;
}

char* chcopy(char* dest,String src,int sizedest){return chcopyt(dest,src,sizedest,false);}//копирование без терминатора
//------------------------------------------------------------------------------------------------------
//ищет строковое значение arg в строке str
String get_argval(String str,String arg,int from/*=0*/,String separators/*=";&"*/)
{
  String val=arg;
  int b=str.indexOf(val+"=",from);
  if(b==-1){
    if((val=="cmd" || val=="c") && (b=str.indexOf("&"))!=-1){//команда может быть без cmd=
      val="";
    }
    else return "";
  }
  b+=(val.length()+1);
  for(int i=0;i<separators.length();i++)
  {
    int e;
    if((e=str.indexOf(separators[i],b))!=-1)
    {
      String r=str.substring(b,e);
      r.trim();
      return r;
    }
  }
  
  String r=str.substring(b);
  r.trim();
  return r;
}
//------------------------------------------------------------------------------------------------------
//сканирование строки на терминальный символ/пробел и заполнение ключей
String scanForKeys(String* s)
{
  unsigned int len=s->length();if(s==0)return "error";
  Serial.print(F("\nscanForKeys: len="));Serial.print(len);
  char c;unsigned int i=0;      
  while(parser->data.Ekeys*8<sizeof(parser->data.Ekey) && i<len)
  {
    uint32_t hs=esp_get_free_heap_size();if(hs<4096){Serial.printf("\n*** FREE HEAP LOW !!! (%zu) ***",hs);/*delay(10000);*/}
    c=s->charAt(i);
    if(c=='\n' || c=='\t' || c==' ' || c==';' || c==','|| c=='"')//разделитель
    {
      if(i!=0)
      {
        String ss=s->substring(0, i);
        if(parser->addKey(ss,false).indexOf("error")!=-1)return "error";
      }
      s->remove(0,i+1);len=len-(i+1);//remove(index,count) - вырезаем часть строки с разделителем
      i=0;continue;
    }
    i++;
  }//while(i<len)
  return "success";
}
//------------------------------------------------------------------------------------------------------
String SWparser::parse_for_keys(uint8_t* buf, size_t len){//сканировать ,буфер на ключи
  if(!len || !buf)return "error: empty";
  //нужна нуль-терминированная строка
  char* m=(char*)malloc(len+1);if(!m)return "error: no allocate memory";
  memset(m,0,len+1);memcpy(m,buf,len);String s=m;
  String resp= scanForKeys(&s);
  free(m);
  return resp;
}
//------------------------------------------------------------------------------------------------------
//в любом случае память перезаписывается размером с выделенную при begin вызовом commit(),
//поэтому нет нужды вычислять адреса отдельно.
void SWparser::init(bool saveData){
  Serial.printf("\nSWparser created as %s",saveData?"SAVE DEFAULT":"ONLY LOAD");
  Serial.print(F("\nSPI_FLASH_SEC_SIZE="));Serial.print(SPI_FLASH_SEC_SIZE);Serial.print(F(" / SWdata size="));Serial.print(sizeof(SWdata));
  EEPROM.begin(sizeof(SWdata));
  uint8_t f=EEPROM.read((int)&data.Eflag-(int)&data);

  bool save=( saveData || (f&FNODEFAULT)==0 );
  
  if(save)
  {
    Serial.print(F("\ninit FLASH"));
    saveAllToEEPROM();
  }
  else loadAllFromEEPROM();

  //get_all_keys(true); only for get in DBG
  
  if(data.Eflag&FSSL || data.Eflag&FSECURE){
    if(!(data.Eflag&FSECURE))Serial.print(F("\n> SSL "));else Serial.print(F("\n>  SSL Secure"));Serial.print(F(" mode"));
    data.EremotePort=443;
    Serial.printf(",set client port=%d",data.EremotePort);
  }
  Serial.print(F("\nAP SSID:"));Serial.print(data.ESSID);Serial.print(F(",pass:"));Serial.print(data.Epass);

  upload=false;
}

void SWparser::loadAllFromEEPROM(){
  //Serial.print("\nload data,size=");Serial.print(sizeof(data));
  data=EEPROM.get((int const)0,data);
  Serial.printf("\nload ok,flags=%s",restore_uint32_t(data.Eflag));
}

void SWparser::saveAllToEEPROM(){
  //Serial.print("\nsave data,size=");Serial.print(sizeof(data));
  data.Eflag|=FNODEFAULT;
  //Serial.printf("\nwrite data,flag=%hhu\n",data.Eflag);
  data=EEPROM.put((int const)0,data);EEPROM.commit();
  Serial.printf("\nwrite ok,flags=%s",restore_uint32_t(data.Eflag));
}
//==============================
//format /cmd?KEY=stringval1&SET/GET/[val]=stringval2 ,return response
String SWparser::process(WebServer* _server/*priority*/,String remote,bool html)
{
   //data.Eflag |= FC_DBG;//ON DEBUG
   
   key=cmd=argv=ext=com="";

   if(parser->data.Eflag & FC_DBG){Serial.printf("\nremote_string=%s",remote.c_str());}
   
   if(_server!=0)
   {
     if(parser->data.Eflag & FC_DBG){Serial.print(F("\n\t uri="));Serial.print(_server->uri());Serial.print(" , args=");Serial.print(_server->args());}
     for(int i=0;i<_server->args();i++) { 
      String argn=_server->argName(i);argn.toLowerCase();
      
      if(parser->data.Eflag & FC_DBG){Serial.printf("\n\t argName(%d)=%s, value=%s",i,_server->argName(i).c_str(),_server->arg(i).c_str());} 
      if(argn=="cmd" || _server->arg(i)==""){
        cmd=_server->arg(i)==""?argn:_server->arg(i);
        if(cmd[0]=='e')cmd.remove(0,1);//cmd=eresetinreval - обрезаем до resetinterval
      }
      else if(argn=="get" || argn=="set"){
          argv=_server->arg(i);
          if(argv[0]=='e')argv.remove(0,1);//eresetinreval - обрезаем до resetinterval
          cmd=argn+String("_")+argv;
          argv="";
      }
      else 
         if(argn=="key")key=_server->arg(i);
           else if(argn=="argv"||argn=="v"||argn=="val"||argn=="value"||(cmd!="" && argn=="data"))argv=_server->arg(i);
             else if(cmd==""){cmd=argn;argv=_server->arg(i);}
              else ext=_server->arg(i);
     }
     
     if(_server->args()<2 || empty(key))return String(F("error: incorrect command"));
   }
   else// удаленка/портал
   {
     if(remote=="")return String(F("error: no remote arguments"));
     remote.replace("%26", "&");remote.replace("%3D", "=");
     cmd=get_argval(remote,"cmd");
     key=get_argval(remote,"key");
     argv=get_argval(remote,"arg");if(argv=="")argv=get_argval(remote,"v");if(argv=="")argv=get_argval(remote,"val");if(argv=="")argv=get_argval(remote,"value");
     ext=get_argval(remote,"ext");
     if(parser->data.Eflag & FC_DBG)Serial.printf("\nremote : key=\"%s\" , cmd=\"%s\" , argv=\"%s\" , ext=\"%s\"",key.c_str(),cmd.c_str(),argv.c_str(),ext.c_str());
   }

   cmd.toLowerCase();
   //----------
   String response="OK";
   String check=data.Ekeys==0?"ISMASTER":checkKey(key);
   if(parser->data.Eflag & FC_DBG){if(data.Ekeys)Serial.printf("\n*** keys total=%d,check=%s",data.Ekeys,check.c_str());}

   if(cmd=="get" || cmd=="set"){
     if(argv=="")return String(F("error: incorrect command"));
     if(argv[0]=='e')argv.remove(0,1);//cmd=get&eresetinreval - обрезаем до resetinterval
     com=cmd+"_"+argv;//заносим в текущую
   }
   
   if(parser->data.Eflag & FC_DBG)Serial.printf("\ncommand : key=\"%s\" , cmd=\"%s\" , argv=\"%s\" , ext=\"%s\"",key.c_str(),cmd.c_str(),argv.c_str(),ext.c_str());
   
   com=cmd;//заносим в текущую
   int v=0;
   //общий доступ==============================>>
   if(1==scom("mac")){String s="device MAC addr is ";s+=mac_toString(mac);return s;}
   else if(scom("check")||scom("checkkey")||scom("key"))return check;
   else if(scom("sync"))return get_sync(check);
   else if(1==scom("v") || 1==scom("ver") || 1==scom("version"))return String(VERSION);  
   else if(1==scom("title") || (1==scom("caption")) || (1==scom("company")))return restore_String(data.Etitle,sizeof(data.Etitle));
   else if(scom("vi")||scom("verinfo")||scom("versioninfo")||scom("vinfo")||scom("version_info")){
     response ="<div class='fx3'>";response+=VERSION_INFO;response+="</div>";return response;
   }
   else if(scom("logo")){response ="<img src=\"data:image/png;base64,"; response += LOGO; response += "\" alt=\"Logo\"/>";return response; }
   else if(1==scom("info")||1==scom("i"))return restore_String(data.Einfo,sizeof(data.Einfo));
   else if(scom("remoteurl"))return getRemoteUrl();
   //закрытый доступ===========================>>
   if(check!="ISMASTER" && check!="FOUND"){ if(parser->data.Eflag & FC_DBG)Serial.printf("\nblocked key:%s=%s",key,check);  return check; }
   else if(cmd=="open"){if(data.Eflag&FNOSERVICE)response="error: service disabled.";else {relayMillis = interval_10ms;_relayOn;}return response;}
   else if(cmd=="close"){if(data.Eflag&FNOSERVICE)response="error: service disabled.";else _relayOff;return response;}
   //мастер====================================>>
   if(check!="ISMASTER")return String(F("error: access denied"));
   else if(cmd=="on"){_relayOn;return response;}else if(cmd=="off"){_relayOff;return response;}
   else if(cmd=="update_firmware"||cmd=="update"||cmd=="ota")
    {
      OTA_enabled=true;
      server->on("/restart", handleRestart);server->on("/reset", handleRestart);
      timerStop(scan_timer);//arduino-esp32/cores/esp32/esp32-hal-timer.h
      Serial.print("\nupdate...");
      String OTA_Index=UPLOAD_HTML;
      OTA_Index.replace("<title></title>","<title>UPDATE BINARY</title>");
      OTA_Index+=POSTHTML;
      server->send(200, "text/html", OTA_Index);
      modAntilock(-1);
      loop_upload(5);//5 min
      OTA_enabled=false;
      response="UPDATE FAILED";    
    }
   else if(cmd=="disable_remote"){data.Eflag|=FNOREMOTE;response="ok.remote control disabled";}
   else if(cmd=="enable_remote"){data.Eflag&=~(uint32_t)FNOREMOTE;response="ok.remote control enabled";}
   else if(cmd=="loadfromhost"||cmd=="loadkeys")
   {
     if(WiFi.status() != WL_CONNECTED)response=F("error : no connection to access point");
     else return loadKeysFromRemoteStorage();
   }
   else if(cmd=="toblack" || cmd=="toblacklist")return toBlackList(argv);
   else if(cmd=="remove" || cmd=="del" || cmd=="deletekey")return deleteKey(argv);
   else if(cmd=="add"||cmd=="addkey")return addKey(argv,true);
   else if(cmd=="clear"||cmd=="clearkeys"){memset(data.Ekey,0,sizeof(data.Ekey));data.Ekeys=0;saveAllToEEPROM();return response="ok.all keys deleted";}
   else if(cmd=="rsttodef"||cmd=="rst_default"||cmd=="init"){
    server_send_ok("<i>...restore default...</i>");server_flush();
    delete parser;
    parser = new SWparser(true);
    esp_restart();
   }
   else if(cmd=="save_toflash" || cmd=="save" || cmd=="savetoeeprom"){saveAllToEEPROM();response="Память обновлена.Текущие настройки сохранены.";}
   else if(cmd=="load" || cmd=="loadfromeeprom"){loadAllFromEEPROM();response="RAM loaded from flash.";}
   else if(cmd=="reset"){server_send_ok("<i>...reset...</i>");server_flush();esp_restart();}//RESET
   
   else if(v=scom("resetinterval")){
    if(v==2){store_uint32_t(data.EresetInterval,argv);saveAllToEEPROM();response="ok.reset interval saved";}else return String(data.EresetInterval);
   }
   else if((v=scom("relayinterval"))||(v=scom("relay"))){
    if(v==2){store_uint32_t(data.ErelayInterval,argv);saveAllToEEPROM();response="ok.relay hold interval saved";}else return String(data.ErelayInterval);
   }
   else if((v=scom("masterkey"))||(v=scom("master"))){
    if(v==2){store_String(data.Emasterkey,sizeof(data.Emasterkey),argv);saveAllToEEPROM();response="ok.masterkey saved";}else return String(data.Emasterkey);     
   }
   else if(v=scom("ssid")){
    if(v==2){store_String(data.ESSID,sizeof(data.ESSID),argv);saveAllToEEPROM();response="ok.AP name saved";}else return restore_String(data.ESSID,sizeof(data.ESSID));
   }
   else if((v=scom("developmentkey"))||(v=scom("devkey"))||(v=scom("dev"))){
    if(v==2){store_String(data.Edevelopment,sizeof(data.Edevelopment),argv);saveAllToEEPROM();response="ok.devkey saved";}else return restore_String(data.Edevelopment,sizeof(data.Edevelopment));
   }
   else if((v=scom("appass"))||(v=scom("pass"))){
    if(v==2){store_String(data.Epass,sizeof(data.Epass),argv);saveAllToEEPROM();response="ok.AP password saved";}else return restore_String(data.Epass,sizeof(data.Epass));
   }
   else if((v=scom("ip"))||(v=scom("apip"))){
    if(v==2){store_IP_String(data.EAPIP,argv);saveAllToEEPROM();return response="ok.AP IP saved";}else return restore_IP_String(data.EAPIP);
   }
   else if((v=scom("port"))||(v=scom("apport"))){
    if(v==2){data.EAPport=argv.toInt();saveAllToEEPROM();return response="ok.AP port saved";}else return String(data.EAPport);
   }
   else if(v=scom("ssidclient")){
    if(v==2){store_String(data.ESSIDClient,sizeof(data.ESSIDClient),argv);saveAllToEEPROM();return response="ok.SSID saved";}else return restore_String(data.ESSIDClient,sizeof(data.ESSIDClient));
   }
   else if((v=scom("passwordclient"))||(v=scom("passclient"))){
    if(v==2){store_String(data.EpasswordClient,sizeof(data.EpasswordClient),argv);saveAllToEEPROM();response="ok.SSID password saved";}else return restore_String(data.EpasswordClient,sizeof(data.EpasswordClient));
   }
   else if((v=scom("host")) || (v=scom("keyshostname")) || (v=scom("keyshost"))){
    if(v==2){store_String(data.Ehost,sizeof(data.Ehost),argv);saveAllToEEPROM();return response="ok.keys hostname saved";}else return restore_String(data.Ehost,sizeof(data.Ehost));
   }
   else if(v=scom("hostfile") || (v=scom("keyshostfile")) || (v=scom("keysfile"))){
    if(v==2){store_String(data.EhostFile,sizeof(data.EhostFile),argv);saveAllToEEPROM();return response="ok.keys file name saved";} else return restore_String(data.EhostFile,sizeof(data.EhostFile));
   }
   else if(v=scom("info")){
    if(v==2){store_String(data.Einfo,sizeof(data.Einfo),argv);saveAllToEEPROM();return response="ok.info saved";}else return restore_String(data.Einfo,sizeof(data.Einfo));
   }
   else if(v=scom("masterdata")){if(v==2)return loadStringMasterdata(_server);else response=sendStringMasterdata(html?"<br>":"&");}
   else if((v=scom("keyshostip"))||(v=scom("keysip"))){if(v==2)store_IP_String(data.EhostIP,argv);else response=restore_IP_String(data.EhostIP);}
   else if((v=scom("keyshostport"))||(v=scom("keysport"))){if(v==2)store_Port(data.EhostPort,argv);else response=String(data.EhostPort);}       
   //-----новые команды--------------
   else if((v=scom("flags"))||(v=scom("mode"))){
    if(v==2)store_uint32_t(data.Eflag,argv);saveAllToEEPROM();
    response=get_flags(html);
   }
   else if(v=scom("remotehost")){if(v==2){store_String(data.EremoteHost,sizeof(data.EremoteHost),argv);saveAllToEEPROM();}else response=restore_String(data.EremoteHost,sizeof(data.EremoteHost));}
   else if(v=scom("remotehostdir")){if(v==2){store_String(data.EremoteHostDir,sizeof(data.EremoteHostDir),argv);saveAllToEEPROM();}else response=restore_String(data.EremoteHostDir,sizeof(data.EremoteHostDir));}
   else if(v=scom("remoteip")){if(v==2){store_IP_String(data.EremoteIP,argv);saveAllToEEPROM();}else response=restore_IP_String(data.EremoteIP);}
   else if(v=scom("remoteport")){if(v==2){store_Port(data.EremotePort,argv);saveAllToEEPROM();}else response=String(data.EremotePort);}
   else if(v=scom("remoteinterval")){if(v==2){data.EremoteIntervalMs=argv.toInt()>=1000?argv.toInt():DEFAULT_REMOTE_INTERVAL;saveAllToEEPROM();} else response= String(data.EremoteIntervalMs);}
   else if(v=scom("requestactualsec")){if(v==2){data.ErequestActualSec=argv.toInt()>=5?argv.toInt():REQUEST_ACTUAL_SEC;saveAllToEEPROM();} else  response=String(data.ErequestActualSec);}
   else if(v=scom("unixtimerefresh")){if(v==2){data.EunixtimeRefresh=argv.toInt()>=5?argv.toInt():UNIXTIME_REFRESH;saveAllToEEPROM();} else  response=String(data.EunixtimeRefresh);}
   else if((v=scom("remotemaxlogs"))||(v=scom("maxlogs"))){if(v==2){data.EremoteMaxLogs=DEFAULT_REMOTE_LOG_LEN;saveAllToEEPROM();}else response=String(data.EremoteMaxLogs);}
   else if((v=scom("remotetimezone"))||(v=scom("timezone"))){if(v==2){store_String(data.EremoteTimeZone,sizeof(data.EremoteTimeZone),argv);saveAllToEEPROM();} else response=restore_String(data.EremoteTimeZone,sizeof(data.EremoteTimeZone));}
   //----------------------
   else if((v=scom("ftpserver"))||(v=scom("ftphost"))||(v=scom("ftp"))){if(v==2){store_String(data.Eftp_server,sizeof(data.Eftp_server),argv);saveAllToEEPROM();} else response=restore_String(data.Eftp_server,sizeof(data.Eftp_server));}
   else if(v=scom("ftpdir")){if(v==2){store_String(data.Eftp_dir,sizeof(data.Eftp_dir),argv);saveAllToEEPROM();} else response=restore_String(data.Eftp_dir,sizeof(data.Eftp_dir));}                        
   else if(v=scom("ftpport")){if(v==2){store_Port(data.Eftp_port,argv);saveAllToEEPROM();}else response=String(data.Eftp_port);}
   else if(v=scom("ftpuser")){if(v==2){store_String(data.Eftp_user,sizeof(data.Eftp_user),argv);saveAllToEEPROM();} else response=restore_String(data.Eftp_user,sizeof(data.Eftp_user));}             
   else if((v=scom("ftppas"))||(v=scom("ftppass"))){if(v==2){store_String(data.Eftp_pass,sizeof(data.Eftp_pass),argv);saveAllToEEPROM();} else response=restore_String(data.Eftp_pass,sizeof(data.Eftp_pass));}          
   else if(v=scom("ftpdelay")){if(v==2){data.Eftp_delay=argv.toInt()>=1000?argv.toInt():5000;saveAllToEEPROM();}else response=String(data.Eftp_delay);}
   else if(cmd=="upload_transponder")return do_upload_transponder_FTP((argv=="set_timezone" || argv=="set_all")?true:false,(argv=="set_maxlogs" || argv=="set_all")?true:false);
   //-------------------------------- 
   else if(v=scom("numkeys"))return String(data.Ekeys);    
   else if(v=scom("maxkeys"))return String((int)MAX_KEYS);    
   else if((v=scom("allkeys"))||(v=scom("keys"))){if(v==2)return "no_realised"; else return get_all_keys();} 
   else if((v=scom("devicename"))||(v=scom("device"))||(v=scom("name"))) //необходимо сначала ответить,а затем сменить имя
   {
     if(v==2){store_String(data.EdeviceName,sizeof(data.EdeviceName),argv);saveAllToEEPROM();}else response=restore_String(data.EdeviceName,sizeof(data.EdeviceName));
   }
   else if(scom("upload")||scom("upload_keys")){//загрузка файла ключей
    upload=true;
    String page=UPLOAD_HTML;
    page.replace("update","upload");
    page.replace("<title></title>","<title>UPLOAD KEYS FILE</title>");
    page+=POSTHTML;
    server->send(200, "text/html", page);
    return "";
   }
   else if((v=scom("title")) || (v=scom("caption")) || (v=scom("company"))){
    if(v==2){v=store_String(data.Etitle,sizeof(data.Etitle),argv);saveAllToEEPROM();return response="ok.captive portal TITLE changed.Stored len="+String(v);} else return restore_String(data.Etitle,sizeof(data.Etitle));
   }
   else if(cmd=="connect"){
      modConCntr(-1);//разрешить отсчёт времени соединения
      WiFi.setAutoConnect(true);
      Serial.printf("\nattempt connecting to %s. . . . ",data.ESSIDClient);
      esp_err_t e = esp_wifi_connect();
      if (e)Serial.printf("\nERROR STA connect=%d", e);
      response="connect to AP...";
   }
   //----------------------------------------------------------------
   else {
    response=String("error: UNKNOWN COMMAND ")+cmd;
    Serial.printf("\ncommand=%s",response.c_str());
   }
  
   return response;
}
//------------------------------------------------------------------------------------------------------
void SWparser::loop_upload(uint32_t minut){
  ULONG te=millis()+minut*60*1000;//5 min
  while(millis()<=te){
    server->handleClient();
    delay(1);
    if (millis() - ledTimeMs1 >= 500) {
      ledTimeMs1 = millis();
      _ledRedToggle;
    }
  }
}
//------------------------------------------------------------------------------------------------------
String SWparser::get_flags(bool html){//get device mode
  uint32_t v=data.Eflag;
//  String s="binary:";
//  for(int i=0;i<=31;i++){
//    s=String((v&1)?"1":"0")+s;v=v>>1;
//  }
  if(html){
    String r="<br>work mode: ";r+=restore_uint32_t(data.Eflag);
    //Eflag bits of SWdata
    r+="<div style=\"text-align:left;\">";
    r+="<br>FNODEFAULT (1)   is ";if(data.Eflag & FNODEFAULT)r+="SET - start without init flash";else r+="reset - start + init flash";   //1 только загрузка,без записи предопределенных значений
    r+="<br>FURL_DBG (2)   is ";if(data.Eflag & FURL_DBG)r+="SET - url debug info";else r+="reset - no url debug info";   //2 выводить информацию о подключении к удаленному серверу
    r+="<br>FNOREMOTE (4)   is ";r+=(data.Eflag & FNOREMOTE)?"SET":"RESET";r+=" - remote over internet ";r+=(data.Eflag & FNOREMOTE)?"disabled":"enabled";   //4 отключить удаленное управление
    r+="<br>FNOANTILOCK (8)   is ";r+=(data.Eflag & FNOANTILOCK)?"SET":"RESET";r+=" - antilocker(CPU HALT) detector ";r+=(data.Eflag & FNOANTILOCK)?"disabled":"enabled";   //8 отключить перезагрузку при обнаружении зависания
    r+="<br>FNOSTACONTROL (16)  is ";r+=(data.Eflag & FNOSTACONTROL)?" - DISABLED":" - enabled";r+=" connections control(max ";r+=String((int)MAX_CLIENTS);r+=(data.Eflag & FNOSTACONTROL)?",maybe problems if overload)":",old connections are deactivated)";   //16 отключить деаунтефикацию при обнаружении лимита подключенных к точке доступа станций
    r+="<br>FSSL (32)  is ";r+=(data.Eflag & FSSL)?"ENABLED - https mode.":"disabled - is http mode.(if enabled,then mode=https)";   //32 HTTPS
    r+="<br>FSECURE (64)  is ";r+=(data.Eflag & FSECURE)?"ENABLED - is secure https mode.(if disabled-insecure)":"disabled - insecure https mode(no CERT control)";   //64 HTTPS Secure mode
    r+="<br>FKSSL (128): keys storage: ";r+=(data.Eflag & FKSSL)?"ENABLED - https mode.":"disabled == http mode.(if enabled,then mode=https)";   //128 HTTPS
    r+="<br>FKSECURE (256): keys storage: ";r+=(data.Eflag & FKSECURE)?"ENABLED - secure https mode.(if disabled-insecure)":"disabled - insecure https mode(no CERT control)";   //256 HTTPS Secure mode
    r+="<br>FNOSERVICE (512) is ";r+=(data.Eflag & FNOSERVICE)?"SET - service is DISABLED for everyone except the master":"reset - the service serves all connected";   //512 отключено обслуживание
    r+="<br>FC_DBG (1024) is ";r+=(data.Eflag & FC_DBG)?"SET":"reset";r+=" - commands debug info is ";r+=(data.Eflag & FC_DBG)?"ENABLED":"disabled";  
//    r+=s;
    r+="</div>";
    return r;  
  }
  else return restore_uint32_t(data.Eflag);
}
//------------------------------------------------------------------------------------------------------
void SWparser::sendChar(char* ar){client_print(ar);};//отослать строку char
//------------------------------------------------------------------------------------------------------
int SWparser::store_String(char* dest_ar,int dest_len,String src_data){
  //if(src_data.length()==0)return "error:null string";
  memset(dest_ar,0,(size_t)dest_len);
  int l=src_data.length();
  if(l!=0){
    if((src_data.charAt(0)=='\"' && src_data.charAt(l-1)=='\"')||(src_data.charAt(0)=='\'' && src_data.charAt(l-1)=='\'')){src_data.remove(0,1);src_data.remove(l-2,1);}
    l=src_data.length();
    if(l>=dest_len){
      Serial.printf("\nstore_String: too long !!! LEN=%d , available %d",l,dest_len);
      l=dest_len-1;
    }
    
    if(l){
      chcopyt(dest_ar,src_data,dest_len);
      //Serial.printf("\nstore_String: stored LEN=%d ,available",l,dest_len);
      return l;
    }
  }

  return 0;
}

String SWparser::restore_String(char* src_ar,int src_len){
  char* ar=(char*)malloc(src_len+1);
  if(!ar){Serial.print("\nrestore_String: error : no memory!!!");return "";}
  memset(ar,0,src_len+1);memcpy(ar,src_ar,src_len);
  String o(ar);free(ar);
  return o;
}
//------------------------------------------------------------------------------------------------------
void SWparser::store_IP_String(char* dest_IP,String src_IP){
  IPAddress adr;adr.fromString(src_IP);
  uint32_t a32=uint32_t(adr);
  memcpy(dest_IP,&a32,sizeof(uint32_t));
}

String SWparser::restore_IP_String(char* src_ar){
  static int ip[4]={src_ar[0],src_ar[1],src_ar[2],src_ar[3]};
  return (String(ip[0])+"."+String(ip[1])+"."+String(ip[2])+"."+String(ip[3]));
}
//------------------------------------------------------------------------------------------------------
uint16_t SWparser::store_Port(uint16_t& dest_Port,String src_Port){
  int tp=src_Port.toInt();
  if(tp>65525||tp<0)tp=80;
  dest_Port=(uint16_t)tp;
  return dest_Port;
}
//------------------------------------------------------------------------------------------------------
uint32_t SWparser::store_uint32_t(uint32_t& dest,String src)
{
  unsigned long number = strtoul(src.c_str(), NULL, 10);
  if(number!=ULONG_MAX)dest=(uint32_t)number;
  return dest;
}

String SWparser::restore_uint32_t(uint32_t src)
{
//  static char dec_s[32];memset(dec_s,0,sizeof(dec_s));sprintf(dec_s,"%ul",src);
//  return String(dec_s);
//return String(dec_s);
  char str[33];memset(str,0,sizeof(str)); /* 11 bytes: 10 for the digits, 1 for the null character */
  //snprintf(str, sizeof str, "%lu", (unsigned long)src); /* Method 1 */
  snprintf(str, sizeof str, "%" PRIu32, src); /* Method 2 */
  return String(str);
}
//------------------------------------------------------------------------------------------------------
String SWparser::substractKey(int index)
{
  if(index*8+8>sizeof(data.Ekey))return "";
  char s[8];
  memcpy(&s[0],&data.Ekey[index*8],7);
  s[7]=0;
  String ss=s;
  return ss;
}

int SWparser::searchKey(String key)
{
  for(int n=0;n<data.Ekeys && n*8<sizeof(data.Ekey);n++){
    String ss=substractKey(n);
    if(ss.equals(key))return n;
  }
  return -1;
}
//---------------
String SWparser::checkKey(String key)
{
  //проверка masterkey1,2
  char s[8];s[7]=0;
  memcpy(&s[0],&data.Edevelopment[0],7);
  String m=s;if(key==m)return String(F("ISMASTER"));
  memcpy(&s[0],&data.Emasterkey[0],7);
  m=s;if(key==m)return String(F("ISMASTER"));
  
  int i=searchKey(key);
  if(i==-1)return String(F("NOTFOUND"));

  int flag=data.Ekey[i*8+7];

  if((flag & 1)==1)return String(F("INBLACK"));
  
  return String(F("FOUND"));
}
//---------------
String SWparser::saveKeyArrayToEEPROM(String arr)
{
  saveAllToEEPROM();
  return String(F("массив ключей записан на флеш."));
}
//---------------
String SWparser::toBlackList(String key)
{
  int i=searchKey(key);
  if(i==-1){
    //return "ERR_NOTFOUND";
    //_server->send(200,"text/raw","ключ не найден.");
    return String(F("COMPLETE"));
  }
  uint8_t f=(data.Ekey[i*8+7] |= 1);
  saveAllToEEPROM();
  return String(F("ключ занесен в чёрный список.Флеш перезаписана."));
}
//---------------
String SWparser::deleteKey(String key)//вернет ответ OK/ERR_ARGVAL-не надено
{
  int i=searchKey(key);
  if(i==-1)return String(F("error: ключ не найден."));
  
  if(i<data.Ekeys-1)//перемещение
  {
    memmove(&data.Ekey[i*8],&data.Ekey[(data.Ekeys-1)*8],8);
  }
  else//стереть
  {
    memset(&data.Ekey[(data.Ekeys-1)*8],0,8);
  }

  data.Ekeys--;
  saveAllToEEPROM();
  
  Serial.printf("\ndeleteKey: data.Ekeys=%d",data.Ekeys);
  
  return String(F("ключ удален.Флеш перезаписана."));
}
//---------------
String SWparser::addKey(String key,bool _saveToEEPROM)//вернет ответ
{
  String k=key;
  if(k=="")return "error: empty value";
  
  if(key.length()>7)
    {
      k=key.substring(0, 7);
      
      Serial.printf("\n\t*** addKey: длина ключа > 7 , копирую только 7 символов : ,\"%s\"",stc(k));
      
    }//if(key.length()>7)
    
  int i=searchKey(k);
  
  Serial.printf("\n\t*** addKey: %s",stc(k));Serial.printf(" search result: %d",i);
  
  if(i>=0){
      Serial.print(F("\n\t*** Ключ найден под индексом."));Serial.print(i);Serial.print(F(" ,отмена."));
      return String(F("error: ключ уже содержится в массиве."));
   }
   
  if((data.Ekeys+1)*8<=sizeof(data.Ekey))
  {
    k.toCharArray(&data.Ekey[data.Ekeys*8],8);data.Ekey[data.Ekeys*8+7]=0;//терминатор//копирование 7 символов
    data.Ekeys++;
  }
  else 
  {
    Serial.printf("\n\t*** addKey: невозможно добавить ключ-достигнуто максимальное количество %d ***",data.Ekeys);
    return String(F("error: невозможно добавить ключ-достигнуто максимальное количество."));
  }

  Serial.printf("\naddKey: data.Ekeys=%d",data.Ekeys);Serial.print(F(" ,added key "));Serial.print(&data.Ekey[(data.Ekeys-1)*8]);
  String s=String("Ключ ")+&data.Ekey[(data.Ekeys-1)*8]+" добавлен.";
  if(_saveToEEPROM)
  {
    saveAllToEEPROM();
    s+="Флеш перезаписана.";
  }
  return s;
}
//=======================    
String SWparser::sendStringMasterdata(String delimiter){

  String sIP=(IPAddress(data.EAPIP[0],data.EAPIP[1],data.EAPIP[2],data.EAPIP[3])).toString();
  
  String s=String(data.EresetInterval)
     + delimiter+String(data.ErelayInterval)
     + delimiter + data.Emasterkey
     + delimiter + data.Edevelopment
     + delimiter + data.ESSID
     + delimiter + data.Epass
     + delimiter + String((int)data.EAPIP[0])         
     + delimiter + String((int)data.EAPIP[1])
     + delimiter + String((int)data.EAPIP[2])
     + delimiter + String((int)data.EAPIP[3])
     + delimiter + String(data.EAPport)  
     + delimiter + data.ESSIDClient
     + delimiter + data.EpasswordClient
     + delimiter + data.Ehost
     + delimiter + data.EhostFile
     + delimiter + data.Einfo
     + delimiter + String(data.Ekeys)
     //--new V4--
     + delimiter + String(data.Eftp_dir)  //256
     + delimiter + String(data.Eftp_user) //64
     + delimiter + String(data.Eftp_pass) //64
     + delimiter + String((int)data.Eftp_port) 
     + delimiter + String((int)data.Eftp_delay) 
     + delimiter + VERSION;
     //----------
     if(data.Eflag&FC_DBG)Serial.printf("\nMASTERDATA ,len=%d ,response=%s",s.length(),s.c_str()); 
     return s;
}
//------------------------------------------------------------------------------------------------------
String SWparser::get_sync(String check_key_value_result){
   String s=restore_String(data.ESSID,sizeof(data.ESSID))+"&"+restore_String(data.Epass,sizeof(data.Epass))+"&"+restore_uint32_t(data.Eflag)+"&"+getRemoteUrl()+"&"+String(VERSION)+"&"+check_key_value_result;
   if(data.Eflag&FC_DBG)Serial.printf("\n->get_sync=%s ,len=%d ,response=",s.c_str(),s.length());
   return s;
}
//------------------------------------------------------------------------------------------------------
String SWparser::getRemoteUrl()
{
    char rIP[32];char rPrt[8];memset(rIP,0,sizeof(rIP));memset(rPrt,0,sizeof(rPrt));
    sprintf(rIP,"%hhu.%hhu.%hhu.%hhu",data.EremoteIP[0],data.EremoteIP[1],data.EremoteIP[2],data.EremoteIP[3]);
    sprintf(rPrt,"%u",(unsigned int)data.EremotePort);
    
  String s=String(data.EdeviceName)+"&"+String(data.EremoteHost)+"&"+String(data.EremoteHostDir)+"&"+String(rIP)+"&"+String(rPrt)  ;
     if(data.Eflag&FC_DBG)Serial.printf("\ngetRemoteUrl=%s ,len=%d ,response=",s.c_str(),s.length()); 
     return s;
}
//------------------------------------------------------------------------------------------------------
//записать структуру без ключей format cmd?key=[keyval]set=masterdata&data=....bytes array....
String SWparser::loadStringMasterdata(WebServer* _server){
  if(_server->args()<3){String ss=F("Error: loadStringMasterdata _server->args()<3");Serial.printf("\n%s",ss);return ss;}
  if(_server->argName(2)!="DATA"){String ss=F("Error: loadStringMasterdata _server->argName(2)!=\"DATA\"");Serial.printf("\n%s",ss);return ss;}
  String d=_server->arg(2);int l=d.length();
  if(l==0){String ss=F("Error: loadStringMasterdata _server.arg[2].length()==0");Serial.printf("\n%s",ss);return ss;}
  Serial.print(F("\nloadStringMasterdata : _server.arg[2]="));Serial.print(d);
  
  //------------------
  int e=1,b=0;int n=0;bool err=false;//Serial.printf("\nsize=%d",sizeof(data.Emasterkey));
  while (e<l)
  {
    if(d.charAt(e)==',' || e+1==l)
    {
      if(e-b>0 || e+1==l)
      {
         String v="";
         if(e+1==l)v=d.substring(b);
         else v=d.substring(b,e);

         //Serial.print("\nloadStringMasterdata: value=");Serial.print(v);
         
         switch (n){
          case 0:data.EresetInterval=intv(v);break;
          case 1:data.ErelayInterval=intv(v);break;
          
          case 2:chcopyt(data.Emasterkey,v,sizeof(data.Emasterkey),true,0);break;
          case 3:if(!v.equals(String(data.Edevelopment))){err=true;}break;//dev
          case 4:chcopyt(data.ESSID,v,sizeof(data.ESSID),true,0);break;
          case 5:chcopyt(data.Epass,v,sizeof(data.Epass),true,0);break;
          case 6:data.EAPIP[0]=intv(v);break;
          case 7:data.EAPIP[1]=intv(v);break;
          case 8:data.EAPIP[2]=intv(v);break;
          case 9:data.EAPIP[3]=intv(v);break;
          case 10:data.EAPport=intv(v);break;
          case 11:chcopyt(data.ESSIDClient,v,sizeof(data.ESSIDClient),true,0);break;
          case 12:chcopyt(data.EpasswordClient,v,sizeof(data.EpasswordClient),true,0);break;
          case 13:chcopyt(data.Ehost,v,sizeof(data.Ehost),true,0);break;
          case 14:chcopyt(data.EhostFile,v,sizeof(data.EhostFile),true,0);break;
          case 15:chcopyt(data.Einfo,v,sizeof(data.Einfo),true,0);break;
          //--new V4--
          case 16:chcopyt(data.Eftp_dir,v,sizeof(data.Eftp_dir),true,0);break;  //256
          case 17:chcopyt(data.Eftp_user,v,sizeof(data.Eftp_user),true,0);break;//64
          case 18:chcopyt(data.Eftp_pass,v,sizeof(data.Eftp_pass),true,0);break;//64
          case 19:data.Eftp_port=uint16_t(intv(v));break;
          case 20:data.Eftp_delay=intv(v);break;
          case 21:if(v!=String(VERSION)){
            loadAllFromEEPROM();//load previous
            String rsp="Ошибка версии.Обновите приложение до версии ";rsp+=VERSION;
            Serial.print(rsp.c_str());
            return rsp;
          }
          //----------
          
          default:break;
       }//switch
       n++;
     }//if(e-b>0 || e+1==l)

     b=e+1;
   }//if(d.charAt(e)==',' || e+1==l)

   e++;
 }while (e<l)

 Serial.printf("\nloadStringMasterdata : n=%d",n);
 if(n<21 || err){
   Serial.print(F("\nloadStringMasterdata : Ошибка загрузки"));
   return String(F("Ошибка загрузки.повторите."));
 }

 
 saveAllToEEPROM();

 if((data.Eflag&FNOREMOTE)!=0){
    STA_action=0;
    if(connected || WiFi.status() == WL_CONNECTED) {
      WiFi.setAutoConnect(false);
      esp_err_t e = esp_wifi_disconnect();
      if (e)Serial.printf("\nERROR esp_wifi_disconnect=%d", e);
   }
 }
 else if(!connected || WiFi.status() != WL_CONNECTED){
   STA_action=3;
   modConCntr(-1);//ждем несколько попыток
   WiFi.setAutoConnect(true);
   esp_wifi_connect();
 }
   
 Serial.print(F("\nloadStringMasterdata : OK"));
 return String(F("Данные записаны в память."));
}
//------------------------------------------------------------------------------------------------------
String SWparser::get_all_keys(bool view){
  String i="\noutput all keys, total=";i=i+data.Ekeys;i=i+" :";Serial.print(i.c_str());
  if(!data.Ekeys)return "no keys";
  i=(!view)?"<br>":"";
  for(int n=0;n<data.Ekeys;n++){
      String ss=substractKey(n);
      if(ss.length()){
        ss=ss+"\r\n";if(!view)ss+="<br>";
        i=i+ss;
      }
  } 
  if(view)Serial.printf("\n*************************\n ALL KEYS: \n%s\n*************************\n",i.c_str());
  return i;
}
//------------------------------------------------------------------------------------------------------
String SWparser::loadKeysFromRemoteStorage(int32_t timeout/*3000*/){
 Serial.print("\n >>> loadKeysFromRemoteStorage:\n");
 int inkeys=data.Ekeys;
 if(inkeys>=MAX_KEYS)return "error: MAX_KEYS";
 if(data.EhostFile=="")return "error: NO FILENAME of keys";
 
 modAntilock(0);;
 String r="",line="",temp="";
 int32_t t=millis()+timeout;
 const char* host=data.Ehost;
 IPAddress ip=IPAddress(data.EhostIP[0],data.EhostIP[1],data.EhostIP[2],data.EhostIP[3]);
 
 bool secure=(data.Eflag&FKSSL)||(parser->data.Eflag&FKSECURE),beginContent=false;
 
 if(secure){
  data.EhostPort=443;Serial.print("\n secure mode,port=443");
  if(!sclient){
    sclient=new WiFiClientSecure();if(!sclient){r=CREATE_CLIENT_FAILED;goto _clear_client_and_exit;}
    if(!(data.Eflag & FKSECURE))sclient->setInsecure();else sclient->setCACert(CERT);// Устанавливаем SSL/TLS сертификат
  }
 }
 else{
   if(!client){
    client=new WiFiClient();
    if(!client){r=CREATE_CLIENT_FAILED;goto _clear_client_and_exit;}
   }
 }

 if(secure){
  if(String(host)!=""){if (!sclient->connect(host, (uint16_t)(data.EhostPort?data.EhostPort:80), timeout) ){r=CONNECTION_FAILED;goto _clear_client_and_exit;}}
  else if(!sclient->connect(ip, (uint16_t)(data.EhostPort?data.EhostPort:80), timeout) ){r=CONNECTION_FAILED;goto _clear_client_and_exit;}
 }
 else {
  if(String(host)!=""){if (!client->connect(host, (uint16_t)(data.EhostPort?data.EhostPort:80), timeout) ){r=CONNECTION_FAILED;goto _clear_client_and_exit;}}
  else if(!client->connect(ip, (uint16_t)(data.EhostPort?data.EhostPort:80), timeout) ){r=CONNECTION_FAILED;goto _clear_client_and_exit;}
 }
 modAntilock(0);
 r=String("GET /") + String(data.EhostFile) + String(" HTTP/1.1\r\nHost: ") +(String(host)==""?ip.toString():String(host)) + String("\r\nConnection: close\r\n\r\n");
 Serial.printf("\nrequest:%s",r.c_str());
 if(secure){if(!sclient->print(r)){r=TRANSFER_ERROR;goto _clear_client_and_exit;}}else {if(!client->print(r)){r=TRANSFER_ERROR;goto _clear_client_and_exit;}}
 //if(!(secure?sclient:client)->print(req))return TRANSFER_ERROR;
 modAntilock(0);
 while( (secure?sclient->available():client->available()) == 0 && millis()<t ){delay(10);Serial.print("\nwaiting response...");}

 if( (secure?sclient->available():client->available()) == 0 && millis()>=t ){
   r=CONNECTION_TIMEOUT;goto _clear_client_and_exit;
 }
 Serial.print("\ngetting response:");
 modAntilock(0);
 while(secure?sclient->available():client->available())
 {
   line=(secure?sclient:client)->readStringUntil('\r');
   //if(data.Eflag&FURL_DBG)
    Serial.print(line.c_str());//received line from server
    
   temp=r=line;temp.toLowerCase();
   
   if(-1!=temp.indexOf("404 not found") || -1!=temp.indexOf("400 bad request")){ r=line;goto _clear_client_and_exit; }
   
   while(line.charAt(0)=='\n'||line.charAt(0)=='\r'||line.charAt(0)=='\t'||line.charAt(0)==' '||line.charAt(0)=='&')line.remove(0,1);//удаление ненужных символов
   
   if(line.length()==0){beginContent=true;Serial.print("\t*** длина строки=0,отмечаю начало файла ***");}
   else if(!beginContent)continue;
   
   if(line.length()!=0)
   {
     if(data.Ekeys>=MAX_KEYS || scanForKeys(&line).indexOf("error")!=-1)
     {
       r="error : ";r+=data.Ekeys>=MAX_KEYS?"MAX KEYS !":"REPEAT KEYS !";
       goto _clear_client_and_exit;
     }
   }
 }//while available

 Serial.print("\nend of response data");
 modAntilock(0);  
 if(inkeys!=data.Ekeys)saveAllToEEPROM();Serial.printf("\n\tKeys total: %d",data.Ekeys);
 r="OK";

 _clear_client_and_exit: //--------------EXIT--------------//
 if(secure){
  sclient->stop();sclient->flush();
 }
 else{
  client->stop();client->flush();
 }
 bool secure0=(data.Eflag&FSSL)||(data.Eflag&FSECURE);
 if(secure0!=secure){
  Serial.printf("\ndelete %sclient...",secure?"secure ":"");
  delete(secure?sclient:client);
  if(secure)sclient=NULL;else client=NULL;
 }
 Serial.printf("\n->result: %s ,keys= %d",r.c_str(),data.Ekeys);
 modAntilock(0);
 return r;
}
//------------------------------------------------------------------------------------------------------
