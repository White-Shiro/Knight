# Knight (Unreal Mecha ARPG Prototype)
### An under-development ARPG prototype developed by WHiTE-SHiRO.

This project uses Medieval-Knight as the maintone of the world and art setting, the combat system took inspiration from a game <b>"Gundam Battle Operation 2"</b>.

We also want shape the presention toward 90s-00s Mecha Anime and the "weighty" feel of how a heavy but not dull humanoid machine should look like. 

![Dev2ThumbNail](https://github.com/user-attachments/assets/e596faad-24c2-4d8d-960d-5e7833c326c0)

[url-unreal-5-4]: https://www.unrealengine.com/en-US/unreal-engine-5
[url-blender-4-2]: https://www.blender.org/download/releases/4-2
## Environment
| Tools            |    Version    |             Download              |
|------------------|:-------------:|:---------------------------------:|
| Unreal Engine    |      5.4 	   |   [Download][url-unreal-5-4] 	   |
| Blender 	   	   |     	4.2 	   |   [Download][url-blender-4-2]	   |

## How to build / play
> Not supported yet. This repository includes only game source code for reference purpose.


## Key feature

### Melee hit detection with async trace interpolation.
![HitBoxInterp](https://github.com/user-attachments/assets/48e1e8b5-513c-473b-be6e-e24dcc9d5eea)
> [!NOTE]
  Yellow Hit Boxes are generated from the AnimationNotify State. [see: HitDetectionNotifyState.h](Source/Knight/Core/Combat/HitDetectionNotifyState.h)
>
>  Blue Hit Boxes are interpated hitbox from the animation frame's gaps. [see: KfMeleeAttackComponent.h](Source/Knight/Core/Character/Knight/KfMeleeAttackComponent.h)


### Cell Shader Outline Shader
A 3 Band Lit Cell Shading that support Texture Map Input and Emisive map, dynamic shadows awared.

Outline is done via Screen Post Processing, using depth and normal edge detection

[![Video](https://img.youtube.com/vi/cpqtP247Z8M/maxresdefault.jpg)](https://www.youtube.com/watch?v=cpqtP247Z8M)
