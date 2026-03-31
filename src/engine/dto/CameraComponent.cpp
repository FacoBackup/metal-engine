#include "CameraComponent.h"

namespace Metal {
    void CameraComponent::registerFields() {
        registerEditableField<FLOAT>(&fov).setName("fov").setLabel("Field of view").setMin(60).setMax(120);
        registerEditableField<FLOAT>(&zNear).setName("zNear").setLabel("Near plane").setMin(0).setMax(1);
        registerEditableField<FLOAT>(&zFar).setName("zFar").setLabel("Far plane").setMin(1).setMax(MAX_INT);

        registerEditableField<FLOAT>(&rotationSensitivity).setName("rotationSensitivity").setLabel("Rotation sensitivity").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&movementSensitivity).setName("movementSensitivity").setLabel("Movement sensitivity").setMin(0).setMax(10);
        registerEditableField<FLOAT>(&zoomSensitivity).setName("zoomSensitivity").setLabel("Zoom sensitivity").setMin(0).setMax(10);

        registerEditableField<BOOLEAN>(&motionBlurEnabled).setName("motionBlurEnabled").setLabel("Motion blur enabled");
        registerEditableField<FLOAT>(&motionBlurVelocityScale).setName("motionBlurVelocityScale").setLabel("Motion blur velocity scale").setMin(0).setMax(500);
        registerEditableField<INT>(&motionBlurMaxSamples).setName("motionBlurMaxSamples").setLabel("Motion blur max samples").setMin(1).setMax(100);
        registerEditableField<BOOLEAN>(&cameraMotionBlur).setName("cameraMotionBlur").setLabel("Camera motion blur enabled");

        registerEditableField<BOOLEAN>(&dofEnabled).setName("dofEnabled").setLabel("Depth of field enabled");
        registerEditableField<FLOAT>(&dofFocusDistance).setName("dofFocusDistance").setLabel("Depth of field focus distance");
        registerEditableField<FLOAT>(&dofAperture).setName("dofAperture").setLabel("Depth of field aperture");
        registerEditableField<FLOAT>(&dofFocalLength).setName("dofFocalLength").setLabel("Depth of field focal length");

        registerEditableField<BOOLEAN>(&bloomEnabled).setName("bloomEnabled").setLabel("Bloom enabled");
        registerEditableField<BOOLEAN>(&filmGrain).setName("filmGrain").setLabel("Film grain enabled");
        registerEditableField<BOOLEAN>(&vignetteEnabled).setName("vignetteEnabled").setLabel("Vignette enabled");
        registerEditableField<BOOLEAN>(&chromaticAberrationEnabled).setName("chromaticAberrationEnabled").setLabel("Chromatic aberration enabled");
        registerEditableField<BOOLEAN>(&distortionEnabled).setName("distortionEnabled").setLabel("Distortion enabled");
        registerEditableField<FLOAT>(&filmGrainStrength).setName("filmGrainStrength").setLabel("Film grain strength").setMin(-MAX_FLOAT).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&vignetteStrength).setName("vignetteStrength").setLabel("Vignette strength").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&bloomThreshold).setName("bloomThreshold").setLabel("Bloom threshold").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<INT>(&bloomQuality).setName("bloomQuality").setLabel("Bloom quality").setMin(0).setMax(MAX_INT);
        registerEditableField<INT>(&bloomOffset).setName("bloomOffset").setLabel("Bloom offset").setMin(0).setMax(MAX_INT);
        registerEditableField<FLOAT>(&chromaticAberrationIntensity).setName("chromaticAberrationIntensity").setLabel("Chromatic aberration intensity").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&distortionIntensity).setName("distortionIntensity").setLabel("Distortion intensity").setMin(0).setMax(MAX_FLOAT);
        registerEditableField<FLOAT>(&yaw).setName("yaw").setLabel("Yaw");
        registerEditableField<FLOAT>(&pitch).setName("pitch").setLabel("Pitch");
        registerEditableField<BOOLEAN>(&mainCamera).setName("mainCamera").setLabel("Is Main Camera");

        registerSerializableOnlyField<BOOLEAN>(&isOrthographic).setName("isOrthographic");
        registerSerializableOnlyField<FLOAT>(&aspectRatio).setName("aspectRatio");
        registerSerializableOnlyField<FLOAT>(&orthographicProjectionSize).setName("orthographicProjectionSize");
    }

    ComponentType CameraComponent::getType() const {
        return CAMERA;
    }
}
